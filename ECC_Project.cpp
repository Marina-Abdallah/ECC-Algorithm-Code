#include <stdio.h>
#include <stdlib.h>
#include <math.h>

//Information about the curve and finite field

int a=4;//coefficient for elliptic curve
int b=20;//coefficient for elliptic curve

//input!!!!!!!!!!!!!!!!!!!!
int p=59;//prime number to provide finite field

int points[1000][2];//to store a set of points satisfying the curve

//Information required for Encryption and Decryption

//Private Information
//input!!!!!!!!!!!!!!!!!!!!!!!
int PrivKey=6;//Private Key of Receiver

//Public Information
int PubKey[2]= {0,0}; //Public key of Receiver
int random=rand();//Random Number required for Encoding
int Pbase[2]= {0,0}; //Base point for all operations

//Encrypted Point
int Enc[4]= {0,0,0,0};

int inverse(int num)
{
    int i=1;
    if (num<0)
        num=p+num;
    for (i=1; i<p; i++)
    {
        if(((num*i)%p)==1)
            break;
    }
    //printf("inverse=%d,%d",i,num);
    return i;
}

int * add(int A[2],int B[2])
{
    int *C =(int*) calloc(2,sizeof(int));
    int x=0;
    if (A[0]==0 || A[1]==0)
    {
        return B;
    }
    if (B[0]==0 || B[1]==0)
    {
        return A;
    }
    if (A[1]==(p-B[1]))
    {
        return C;
    }
    if ((A[0]==B[0]) && (A[1]==B[1]))
    {
        x=((3*(A[0]*A[0]))+a)*inverse(2*A[1]);
        C[0]=((x*x)-(2*A[0]))%p;
        C[1]=((x*(A[0]-C[0]))-A[1])%p;
        //C[0]=((A[0]*A[0])%p+(b*inverse(A[0]*A[0]))%p)%p;//For Binary Curves
        //C[1]=((A[0]*A[0])%p+((A[0]+(A[1]*inverse(A[0]))*C[0]))%p+(C[0])%p)%p;//For Binary Curves
    }
    else
    {
        x=(B[1]-A[1])*inverse(B[0]-A[0]);
        C[0]=((x*x)-(A[0]+B[0]))%p;
        C[1]=((x*(A[0]-C[0]))-A[1])%p;
        //C[0]=((((A[1]+B[1])*inverse(A[0]+B[0]))*((A[1]+B[1])*inverse(A[0]+B[0])))%p + ((A[1]+B[1])*inverse(A[0]+B[0]))%p + A[0]%p + B[0]%p + a%p)%p;//For Binary Curves
        //C[1]=((((A[1]+B[1])*inverse(A[0]+B[0]))*(A[0]+C[0]))+C[0]+A[1])%p;//For Binary Curves
    }
    if (C[0]<0)
        C[0]=p+C[0];
    if (C[1]<0)
        C[1]=p+C[1];
    return C;
}


int * sclr_mult(int k,int P[2])//using LSB first algorithm
{
    int *temp,i;
    int *Q =(int*) calloc(2,sizeof(int));
    Q[0]=0;
    Q[1]=0;
    for(i=31; i>=0; i--)
    {
        if((k>>i)&1)
            break;
    }
    for(int j=0; j<=i; j++)
    {
        if((k>>j)&1)
        {
            temp=add(Q,P);
            Q[0]=*temp;
            Q[1]=*(temp+1);
        }
        temp=add(P,P);
        P[0]=*temp;
        P[1]=*(temp+1);
    }
    return Q;
}



void generate()
{
    int rhs,lhs,i=0;//to find set of points that satisfy the elliptic curve
    for(int x=0; x<p; x++)
    {
        rhs=((x*x*x)+(a*x)+b)%p;
        for(int y=0; y<p; y++)
        {
            lhs=(y*y)%p;
            if (lhs==rhs)
            {
                points[i][0]=x;
                points[i][1]=y;
                i+=1;
            }
        }
    }
    printf("\nNumber of points found on the curve is %d \n",i);
    for(int k=0; k<i; k++)
    {
        printf("%d(%d,%d)\n",(k),points[k][0],points[k][1]);
    }
}

int * genKey(int X,int P[2])
{
    int *temp;
    int *Q =(int*)calloc(2,sizeof(int));
    //compute public key
    temp=sclr_mult(X,P);
    Q[0]=*temp;
    Q[1]=*(temp+1);
    return Q;
}

//Functions Used
/*int * sclr_mult(int k,int point[2]);
int * add(int A[2],int B[2]);
int inverse(int num);
int * encode(int m,int Pb[2],int random,int Pbase[2]);//(Message,Public Key)
int * genKey(int X,int P[2]);//(Private Key,Base Point)
int decode(int Enc[4],int PrivKey);//(Encrypted Message, Private key of the Receiver) Outputs Message
void generate();*/

int main()
{
    int dataSet=1;
    for(int i=0; i<dataSet; i++)
    {
        int *temp;

        generate();
        //input!!!!!!!!!!
        // m7taga q3da baiza seka!!!!!!!!!!!!!!!
        Pbase[0]=points[1][0];//Deciding the base point here
        Pbase[1]=points[1][1];

        temp=genKey(PrivKey,Pbase);
        PubKey[0]=*temp;
        PubKey[1]=*(temp+1);
        printf("\nThe Public Key is (%d,%d)\n",PubKey[0],PubKey[1]);

        //input!!!!!!!!!!!!!!
        int message[2];
        message[0]=points[5][0];
        message[1]=points[5][1];
        printf("The message point is (%d,%d)\n",message[0],message[1]);
        // encrytion
        int P[2];
        temp=sclr_mult(random,Pbase);
        P[0]=*temp;
        P[1]=*(temp+1);

        int Q[2];
        temp=sclr_mult(random,PubKey);
        Q[0]=*temp;
        Q[1]=*(temp+1);

        int R[2];
        temp=add(message,Q);
        R[0]=*temp;
        R[1]=*(temp+1);

        printf("The encrypted point is [(%d,%d),(%d,%d)]\n",P[0],P[1],R[0],R[1]);

        //decryption
        temp=sclr_mult(PrivKey,P);
        int O[2];
        O[0]=*temp;
        O[1]=p-*(temp+1);

        temp=add(R,O);
        O[0]=*temp;
        O[1]=*(temp+1);
        printf("The message point is (%d,%d)\n",O[0],O[1]);
    }
    return 0;
}


