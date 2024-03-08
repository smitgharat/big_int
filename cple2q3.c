#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_DIGITS 1024

typedef struct bg
{
    int digits[MAX_DIGITS];
    int num_digits;
} BigInt;

// Helper function to initialize a BigInt with a value of 0
void initialize(BigInt* n) 
{
    int i;
    for (i = 0; i < MAX_DIGITS; i++) 
    {
        n->digits[i] = 0;
    }
    n->num_digits = 0;
}

// Helper function to remove leading zeros from a BigInt
void remove_leading_zeros(BigInt* n) 
{
    while (n->num_digits > 1 && n->digits[n->num_digits - 1] == 0) 
    {
        n->num_digits--;
    }
}

// Constructor to create a BigInt from a long long integer
void from_long_long(BigInt* n, long long value) 
{
    initialize(n);
    while (value > 0) 
    {
        n->digits[n->num_digits++] = value % 10;
        value /= 10;
    }
}

// Constructor to create a BigInt from a string
void from_string(BigInt* n, const char* str) 
{
    initialize(n);
    int len = strlen(str);
    for (int i = len - 1; i >= 0; i--) 
    {
        if (str[i] < '0' || str[i] > '9') 
        {
            fprintf(stderr, "Error: Invalid input string.\n");
            exit(EXIT_FAILURE);
        }
        n->digits[n->num_digits++] = str[i] - '0';
    }
    remove_leading_zeros(n);
}

// Constructor to create a BigInt from another BigInt
void from_bigint(BigInt* n, const BigInt* other) 
{
    memcpy(n, other, sizeof(BigInt));
}

// Function to print a BigInt to stdout
void print(const BigInt* n) 
{
    for (int i = n->num_digits - 1; i >= 0; i--) 
    {
        printf("%d", n->digits[i]);
    }
    printf("\n");
}

// Comparison functions

int compare(const BigInt* a, const BigInt* b) 
{
    if (a->num_digits > b->num_digits) 
    {
        return 1;
    } 
    else if (a->num_digits < b->num_digits) 
    {
        return -1;
    } 
    else 
    {
        for (int i = a->num_digits - 1; i >= 0; i--) 
        {
            if (a->digits[i] > b->digits[i]) 
            {
                return 1;
            } 
            else if (a->digits[i] < b->digits[i]) 
            {
                return -1;
            }
        }
        return 0;
    }
}

int equals(const BigInt* a, const BigInt* b) 
{
    return compare(a, b) == 0;
}

// Addition function
void add(BigInt* result, const BigInt* a, const BigInt* b) 
{
    initialize(result);

    int carry = 0;
    int max_num_digits = a->num_digits > b->num_digits ? a->num_digits : b->num_digits;
    for (int i = 0; i < max_num_digits; i++) 
    {
        int sum = a->digits[i] + b->digits[i] + carry;
        result->digits[i] = sum % 10;
        carry = sum / 10;
        result->num_digits++;
    }

    if (carry > 0) 
    {
        result->digits[result->num_digits++] = carry;
    }
    remove_leading_zeros(result);
}

void subtract(BigInt* result, const BigInt* a, const BigInt* b) 
{
    initialize(result);

    if (compare(a, b) < 0) 
    {
        fprintf(stderr, "Error: Cannot perform subtraction, result will be negative.\n");
        exit(EXIT_FAILURE);
    }

    int i = 0, cur_dig_a = a->digits[0], cur_dig_b = b->digits[0];
    while(i < a->num_digits)
    {
        if(cur_dig_a < cur_dig_b)
        {
            result->digits[i] = cur_dig_a + 10 - cur_dig_b;
            result->num_digits++;
            //printf("%d", result->digits[i]); //just to check
            cur_dig_a = i+1 < a->num_digits ? a->digits[i+1] - 1 : 0;
            cur_dig_b = i+1 < a->num_digits ? b->digits[i+1] : 0;
            i++;
        }
        else
        {
            result->digits[i] = cur_dig_a - cur_dig_b;
            result->num_digits++;
            //printf("%d", result->digits[i]); //just to check
            cur_dig_a = i+1 < a->num_digits ? a->digits[i+1] : 0;
            cur_dig_b = i+1 < a->num_digits ? b->digits[i+1] : 0;
            i++;
        }
    }
    // Remove any leading zeros from the result
    remove_leading_zeros(result);
}

BigInt multiply(BigInt* result, const BigInt* a, const BigInt* b) 
{
    initialize(result);
    int m = a->num_digits, n = b->num_digits;
    int i, j, carry;
    for (i = 0; i < n; i++) 
    {
        carry = 0;
        BigInt temp;
        initialize(&temp);
        for (j = 0; j < i; j++) 
        {
            temp.digits[j] = 0;
        }
        for (j = 0; j < m; j++) 
        {
            int prod = b->digits[i] * a->digits[j] + carry;
            temp.digits[j + i] = prod % 10;
            carry = prod / 10;
        }
        if (carry > 0) 
        {
            temp.digits[j + i] = carry;
            temp.num_digits = j + i + 1;
        } 
        else 
        {
            temp.num_digits = j + i;
        }
        BigInt temp2;
        initialize(&temp2);
        add(&temp2, result, &temp);
        from_bigint(result, &temp2);
    }
    return *result;
}

void increment(BigInt *q)
{
    int carry = 1, i = 0;
    while(carry && i < q->num_digits)
    {
        int sum = q->digits[i] + carry;
        q->digits[i] = sum % 10;
        carry = sum / 10;
        i++;
    }
    if(carry)
    {
        q->digits[q->num_digits++] = carry;
    }
}

// Function to divide two BigInts and return the quotient and remainder
void divide(BigInt* q, BigInt* r, const BigInt* a, const BigInt* b) 
{
    BigInt dividand, a_helper;
    initialize(q);
    initialize(r);
    //initialize(&dividand);
    initialize(&a_helper);
    from_bigint(&a_helper, a);

    // print(&dividand);
    // print(&divisor);

    if(compare(a, b) < 0)
    {
        print(a);
        return;
    }

    while(compare(&dividand, b) >= 0)
    {
        subtract(&dividand, &a_helper, b);
        from_bigint(&a_helper, &dividand);
        //print(&dividand);
        increment(q);
    }
    from_bigint(r, &dividand);
}

void squareRoot(BigInt *result, const BigInt *a)
{
    BigInt temp;
    initialize(&temp);
    initialize(result);
    while(compare(result, a) < 0)
    {
        increment(&temp);
        multiply(result, &temp, &temp);
    }
    from_bigint(result, &temp);
}

void power(BigInt *result, const BigInt *a, int pow)
{
    int i;
    BigInt temp;
    initialize(&temp);
    from_bigint(&temp, a);
    for(i=0; i<pow-1; i++)
    {
        multiply(result, &temp, a);
        from_bigint(&temp, result);
    }
    from_bigint(result, &temp);
}

void convert(BigInt *result, long long x)
{
    initialize(result);
    while(x)
    {
        result->digits[result->num_digits++] = x % 10;
        x /= 10;
    }
}

void factorial(BigInt *result, const BigInt *a)
{
    BigInt temp, mul;
    initialize(&temp);
    initialize(&mul);
    increment(&mul);
    while(compare(&temp, a) != 0)
    {
        increment(&temp);
        multiply(result, &mul, &temp);
        from_bigint(&mul, result);
    }
}

int main() 
{
    BigInt a, b, result, rem;
    char str1[MAX_DIGITS], str2[MAX_DIGITS];
    long long x;
    int choice, pow;
    do
    {
        /* code */
        printf("\n\n***AVAILABLE OPERATIONS**\n\n");
        printf("\n1. Modulo of two big integers");
        printf("\n2. Square root of a big integer");
        printf("\n3. Raise a big integer to a power");
        printf("\n4. Convert a simple integer to a big integer");
        printf("\n5. Calculate factorial of a big integer");
        printf("\n6. EXIT");
        printf("\nEnter your choice : ");
        scanf("%d", &choice);

        switch (choice)
        {
        case 1:
            printf("\nEnter two big integers: ");
            scanf("%s", str1);
            scanf("%s", str2);
            from_string(&a, str1);
            from_string(&b, str2);
            divide(&result, &rem, &a, &b);
            printf("\nReminder is");
            print(&rem);
            break;

        case 2:
            printf("\nEnter a big integer: ");
            scanf("%s", str1);
            from_string(&a, str1);
            squareRoot(&result, &a);
            printf("\n answer = " );
            print(&result);
            break;

        case 3:
            printf("\nEnter a big integer: ");
            scanf("%s", str1);
            from_string(&a, str1);
            printf("\nEnter pow to raise this big int: ");
            scanf("%d", &pow);
            power(&result, &a, pow);
            printf("\n answer =");
            print(&result);
            break;

        case 4:
            printf("\nEnter a sample integer: ");
            scanf("%lli", &x);
            convert(&result, x);
            printf("\n answer = ");
            print(&result);
            break;
        
        case 5:
            printf("\nEnter a big integer: ");
            scanf("%s", str1);
            from_string(&a, str1);
            factorial(&result, &a);
            printf("\n answer =");
            print(&result);
            break;

        case 6:
            printf("\nExiting...");
            break;

        default:
            printf("\nInvalid choice made!!!");
            break;
        }

    } while (choice != 6);

    return 0;
}