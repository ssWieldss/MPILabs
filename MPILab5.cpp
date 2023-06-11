#include <iostream>
#include <string>
#include <mpi.h>
#include <locale.h> 
#include <sstream>
#define _CRT_SECURE_NO_WARNING
#include <regex>

#define max(a,b) ((a) > (b) ? (a) : (b))

using namespace std;

string findSum(string str1, string str2)
{
    if (str1.length() > str2.length())
        swap(str1, str2);

    string str = "";

    int n1 = str1.length();
    int n2 = str2.length();

    reverse(str1.begin(), str1.end());
    reverse(str2.begin(), str2.end());

    int carry = 0;
    for (int i = 0; i < n1; i++) {

        int sum
            = ((str1[i] - '0')
                + (str2[i] - '0')
                + carry);
        str.push_back(sum % 10 + '0');

        carry = sum / 10;
    }

    for (int i = n1; i < n2; i++) {
        int sum = ((str2[i] - '0') + carry);
        str.push_back(sum % 10 + '0');
        carry = sum / 10;
    }

    if (carry)
        str.push_back(carry + '0');

    reverse(str.begin(), str.end());

    return str;
}

string findDiff(string str1, string str2)
{
    string str = "";

    int n1 = str1.length(), n2 = str2.length();

    reverse(str1.begin(), str1.end());
    reverse(str2.begin(), str2.end());

    int carry = 0;

    for (int i = 0; i < n2; i++) {

        int sub
            = ((str1[i] - '0')
                - (str2[i] - '0')
                - carry);

        if (sub < 0) {
            sub = sub + 10;
            carry = 1;
        }
        else
            carry = 0;

        str.push_back(sub + '0');
    }

    for (int i = n2; i < n1; i++) {
        int sub = ((str1[i] - '0') - carry);

        if (sub < 0) {
            sub = sub + 10;
            carry = 1;
        }
        else
            carry = 0;

        str.push_back(sub + '0');
    }

    reverse(str.begin(), str.end());

    return str;
}

string removeLeadingZeros(string str)
{
    const regex pattern("^0+(?!$)");

    str = regex_replace(str, pattern, "");
    return str;
}

string multiply(string A, string B)
{

    if (A.length() > B.length())
        swap(A, B);

    int n1 = A.length(), n2 = B.length();
    while (n2 > n1) {
        A = "0" + A;
        n1++;
    }

    if (n1 == 1) {

        int ans = stoi(A) * stoi(B);
        return to_string(ans);
    }

    if (n1 % 2 == 1) {
        n1++;
        A = "0" + A;
        B = "0" + B;
    }

    string Al, Ar, Bl, Br;

    for (int i = 0; i < n1 / 2; ++i) {
        Al += A[i];
        Bl += B[i];
        Ar += A[n1 / 2 + i];
        Br += B[n1 / 2 + i];
    }

    string p = multiply(Al, Bl);

    string q = multiply(Ar, Br);

    string r = findDiff(multiply(findSum(Al, Ar), findSum(Bl, Br)), findSum(p, q));

    for (int i = 0; i < n1; ++i)
        p = p + "0";

    for (int i = 0; i < n1 / 2; ++i)
        r = r + "0";

    string ans = findSum(p, findSum(q, r));

    ans = removeLeadingZeros(ans);

    return ans;
}

int main(int* argc, char** argv) {
    int size = 16, SIZE = size * 4;
    int numtasks, rank;
    int* incl_group = new int[2]{ 0, 1 };


    MPI_Status status;
    MPI_Init(argc, &argv);

    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &numtasks);

    int first_size, first_rank, second_size, second_rank;

    MPI_Group global_group;
    MPI_Group first_group, second_group;
    MPI_Comm my_comm1, my_comm2;
    MPI_Comm_group(MPI_COMM_WORLD, &global_group);
    MPI_Group_incl(global_group, 2, incl_group, &first_group);
    MPI_Comm_create(MPI_COMM_WORLD, first_group, &my_comm1);
    MPI_Group_size(first_group, &first_size);
    MPI_Group_rank(first_group, &first_rank);
    MPI_Group_excl(global_group, 2, incl_group, &second_group);
    MPI_Comm_create(MPI_COMM_WORLD, second_group, &my_comm2);
    MPI_Group_size(second_group, &second_size);
    MPI_Group_rank(second_group, &second_rank);

    string  res;
    string astr1, astr2, astr3;
    long long aint1 = 12345, aint2 = 12345;
    long long result;

    if (first_rank == 0) {
        MPI_Send(&aint1, 1, MPI_UNSIGNED_LONG_LONG, 1, 0, my_comm1);
        printf("\n(com-1) Process %2d --> 1", first_rank);
    }
    if (first_rank == 1) {
        MPI_Recv(&aint1, 1, MPI_UNSIGNED_LONG_LONG, 0, 0, my_comm1, MPI_STATUS_IGNORE);
        printf("\n(com-1) Process %2d --> 0", first_rank);

        astr1 = to_string(aint1);
        astr2 = to_string(aint2);

        cout << "\n" << astr1 << " * " << astr2;
        res = multiply(astr1, astr2);

        result = stoll(res);

        MPI_Send(&result, 1, MPI_UNSIGNED_LONG_LONG, 0, 0, MPI_COMM_WORLD);
        printf("\n(com-1) Process %2d --> 0 (global)", first_rank);
    }
    if (second_rank == 0) {
        MPI_Send(&aint2, 1, MPI_UNSIGNED_LONG_LONG, 1, 0, my_comm2);
        printf("\n(com-2) Process %2d -> 1", second_rank);
    }
    if (second_rank == 1) {
        MPI_Recv(&aint2, 1, MPI_UNSIGNED_LONG_LONG, 0, 0, my_comm2, MPI_STATUS_IGNORE);
        printf("\n(com-2) Process %2d -> 0", second_rank);

        astr2 = to_string(aint2);
        astr1 = to_string(aint1);

        cout << "\n" << astr2 << " * " << astr3;
        res = multiply(astr2, astr3);

        result = stoll(res);

        MPI_Send(&result, 1, MPI_UNSIGNED_LONG_LONG, 0, 0, MPI_COMM_WORLD);
        printf("\n(com-2) Process %2d -> 0 (global)", second_rank);
    }
    if (rank == 0) {
        MPI_Recv(&aint1, 1, MPI_UNSIGNED_LONG_LONG, 1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        MPI_Recv(&aint2, 1, MPI_UNSIGNED_LONG_LONG, 3, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        cout << "\n aint1 = " << aint1;
        cout << "\n aint2 = " << aint2;

        astr1 = to_string(aint1);
        astr2 = to_string(aint2);

        res = multiply(astr1, astr2);
        cout << "\n result = " << res;
    }
    MPI_Group_free(&global_group);
    MPI_Group_free(&first_group);
    MPI_Group_free(&second_group);

    if (MPI_COMM_NULL != my_comm1) {
        MPI_Comm_free(&my_comm1);
    }
    if (MPI_COMM_NULL != my_comm2) {
        MPI_Comm_free(&my_comm2);
    }
    MPI_Finalize();
    return 0;
}


