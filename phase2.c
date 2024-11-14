#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
struct processcontrolblock{
    int ttl;
    int jobid;
    int tll;
    int ttc;
    int llc;
};
struct processcontrolblock pcb;
char memory[300][4];
char buffer[40];
// char buf2[40];
char ir[4];
int ic = 0, si = 0,em,pi=0,ti=0;
bool c = false,fault=false,error=false,valid=false;
int value[30],ind=0,key[30],kind=0,addrs;
char r[4];
int ptr,pte,pg;
int va=0,ra=0;
FILE *inputf;
FILE *outputf;
int state[30];
void executeprogram();
void mos();
void address();
void terminate(int n);
void clearBuffer()
{
    for (int m = 0; m < 40; m++)
    {
        buffer[m] = '\0';
        //buf2[40] = '\0';
    }
}

int allocate(){
int num=rand()%30;
if(state[num]==0){
    state[num]=1;
    return num;
}
else{
    allocate();
}
}

void inipgtable(){
    for(int i=ptr;i<(ptr+10);i++){
        for(int j=0;j<4;j++){
            memory[i][j]='*';
        }
    }
    for(int i=0;i<30;i++){
        state[i]=0;
    }
}

int map(int v){
    if((ir[0]!='H' && ir[1]!='\0') || (ir[0]!='B' && ir[0]!='T')){
   for(int i=0;i<=kind;i++){
    printf("kind=%d\n",kind);
    if(key[i]==(v/10)*10){
        return (value[i]*10)+(v%10);
    }
   }
   pi=3;
   mos();
   return addrs;
   }
}

void address(){
    //printf("va=%d ptr=%d\n",va,ptr);
    pte=ptr+va/10;
    //printf("pte=%d \n",pte);
    // if(memory[pte][2]==''|| memory[pte][3]==''){
    //     pi=3;
    //     mos();
    //     return;
    // }
    int num=(memory[pte][2]-'0')*10+memory[pte][3]-'0';
    ra=(num*10)+(va%10);
   // printf("ra=%d ",ra);
    }

void read()
{
    //inputf = fopen("in.txt", "r");
    // va = (ir[2] - '0') * 10 + (ir[3] - '0');
    // address();
    //printf("reached read part\n");
    // if(fault==true){
    //     int d=allocate();
    //     memory[pte][2]=(d/10)+'0';
    //     memory[pte][3]=(d%10)+'0';
    //     va = (ir[2] - '0') * 10 + (ir[3] - '0');
    //     address();
    //     fault=false;
    //     pg++;
    // }

    //printf("ra=%d ",ra);
    int k = 0, p = 0;
    clearBuffer();
    fgets(buffer, 41, inputf);
    if(buffer[0]=='$' && buffer[1]=='E' && buffer[2]=='N' && buffer[3]=='D'){
        terminate(1);
        return;
    }
  
        while (k < 40 && buffer[k] != '\0')
    {
        for (int j = 0; j < 4; j++)
        {
            if(buffer[k]=='\n'){
                k++;
                continue;  
            }
            memory[ra][j] = buffer[k];
            ++k;
        }
        ++ra;
    }
    
    
    clearBuffer();
    si=0;
    //fclose(inputf);
    return;
}
void write()
{
    outputf = fopen("output2.txt", "a");
      //printf("reached write part\n");
   
    pcb.llc=pcb.llc+1;
    //printf("llc=%d\n",pcb.llc);
    if(pcb.llc>pcb.tll){
        terminate(2);
        return;
    }
    else{
        // va=(ir[2] - '0') * 10+ir[3]-'0';
        // address();
        // if(fault==true){
        //     pi=3;
        //     mos();
        //     return;
        // }
        //printf("ra=%d ",ra);
        for (int i = ra; i <= ((ra+9)); i++)
    {
        //printf("i in write=%d\n",i);
        for (int k = 0; k < 4; k++)
        {
            if(memory[i][k]=='\0'){
                fprintf(outputf,"%c",' ');
            }
            else
            {
                fprintf(outputf, "%c", memory[i][k]);
            }
        }
    }
    si=0;
    fprintf(outputf,"%c",'\n');
    fclose(outputf);
    
    }
    return;
}
void terminate(int n)
{   //printf("reached terminate\n");
    outputf = fopen("output2.txt", "a");
    fprintf(outputf, "\n");
    fprintf(outputf, "\n");
    if(n==0){
        fprintf(outputf,"No Error\n");
    }
     else if(n==1){
        fprintf(outputf,"Out of Data\n");
    }
    else if(n==2){
        fprintf(outputf,"Line Limit Exceeded\n");
    }
    else if(n==3){
        fprintf(outputf,"Time Limit Exceeded\n");
    }
    else if(n==4){
        fprintf(outputf,"OpCode Error\n");
    }
    else if(n==5){
        fprintf(outputf,"Operand Error\n");
    }
    else if(n==6){
        fprintf(outputf,"Invalid Page Fault\n");
    }

    if(n!=0){
        //printf("n=%d",n);
         error=true;
    }
   
    si = 0,ti=0,pi=0;
    fprintf(outputf, "ir=%c%c%c%c\n", ir[0],ir[1],ir[2],ir[3]);
    fprintf(outputf, "jobid=%d\n", pcb.jobid);
    fprintf(outputf, "ttl=%d\n", pcb.ttl);
    fprintf(outputf, "ttc=%d\n", pcb.ttc);
    fprintf(outputf, "tll=%d\n", pcb.tll);
    fprintf(outputf, "llc=%d\n", pcb.llc);
    fclose(outputf);
}
void mos()
{

    printf("si=%d pi=%d ti=%d \n",si,pi,ti);
    if (si == 1 && ti==0)
    {
        read();
    }
    else if (si == 2 && ti==0)
    {
        write();
    }
    else if (si == 3 && ti==0)
    {
        terminate(0);
    }
    else if(ti==2 && si==1){
        terminate(3);
    }
    else if(ti==2 && si==2){
        write();
        terminate(3);
    }
    else if(ti==2 && si==3){
        terminate(0);
    }
    else if(ti==0 && pi==1){
        terminate(4);
    }
    else if(ti==0 && pi==2){
        terminate(5);
    }
    else if(ti==0 && pi==3){
        //executeprogram();
        if((ir[0]=='G' && ir[1]=='D') ||(ir[0]=='S' && ir[1]=='R')){
            int d=allocate();
            kind++;
            value[kind]=d;
            key[kind]=((ir[2]-'0')*10+ir[3]-'0');
            memory[ptr+pg][2]=(d/10)+'0';
            memory[pte+pg][3]=(d%10)+'0';
        //     va=(ir[2]-'0')*10+ir[3]-'0';
        //    address();
        addrs=value[kind]*10;
           printf("pg=%d",pg);
            pg++;
            pi=0;
            
            return;
        }else {
            terminate(6);
            }
    }
     
    if(error==true){
         return;
    }
   //si = 0,ti=0,pi=0;
}
void simulation(){
    //printf("simulation\n");
    if(pcb.ttc>pcb.ttl){
        ti=2;
        si=0;
        mos();
      } 
      if((ir[0]=='G'&&ir[1]=='D') || (ir[0]=='S'&&ir[1]=='R'))
      pcb.ttc+=2;
  
      else if((ir[0]=='P'&&ir[1]=='D') || (ir[0]=='C'&&ir[1]=='R') || (ir[0]=='L'&&ir[1]=='R') || (ir[0]=='B'&&ir[1]=='T') || (ir[0]=='H') ){
        pcb.ttc++;
      }

      printf("ttc=%d\n",pcb.ttc);
      if(pcb.ttc>pcb.ttl){
        ti=2;
        mos();
      } 
}
void executeprogram()
{
    // ic = 0;
    // va=ic;
    //     address();
    while (ic < 99 )
    {
        //printf("ic=%d\n",ic);
        va=ic;
        address();
        printf("ra=%d\n",ra);
        //if( memory[ra][0] != '\0'){
            for (int i = 0; i < 4; i++)
        {
            ir[i] = memory[ra][i];
        }
        ++ic;
        //simulation();
        printf("%c%c%c%c\n",ir[0],ir[1],ir[2],ir[3]);
        if(ir[0]!='H'){
            if( ( !((ir[2]-'0')>=0 && (ir[2]-'0')<=9)|| !((ir[3]-'0')>=0 && (ir[3]-'0')<=9)) || (((ir[2]-'0')*10+(ir[3]-'0'))<0 || ((ir[2]-'0')*10+(ir[3]-'0'))>99)){
            printf("%c%c%c%c\n",ir[0],ir[1],ir[2],ir[3]);
            pi=2;
            mos();
            //error=true;
            return;
        }
        }
         if (ir[0] == 'G' && ir[1] == 'D')
        {   //valid=true;
        //     va=(ir[2]-'0')*10+ir[3]-'0';
        //    // va=pg;
        //     address();
            addrs=map((ir[2]-'0')*10+ir[3]-'0');
            ra=addrs;
            si = 1;
            simulation();
            mos();
        }
        else if (ir[0] == 'P' && ir[1] == 'D')
        {
            // if(fault==true){
            //     terminate(6);
            //     break;
            // }
            // valid=false;
            
            // va=(ir[2]-'0')*10+ir[3]-'0';
            // address();
            addrs=map((ir[2]-'0')*10+ir[3]-'0');
            ra=addrs;
            si = 2;
            mos();
            simulation();
        }
        else if (ir[0] == 'H' && ir[1]=='\0')
        {
            si = 3;
            simulation();
            mos();
            break;
        }
        else if (ir[0] == 'L' && ir[1] == 'R')
        {  // valid =false;
        //     va = (ir[2] - '0') * 10 + (ir[3] - '0');
        //     address();
        addrs=map((ir[2]-'0')*10+ir[3]-'0');
            ra=addrs;
            printf("ra=%d\n",ra);
            for (int j = 0; j < 4; j++)
            {
                r[j] = memory[ra][j];
            }
        }
        else if (ir[0] == 'S' && ir[1] == 'R')
        {   //valid=true;
        //     va = (ir[2] - '0') * 10 + (ir[3] - '0');
        //     address();
    //         if(fault==true){
    //     int d=allocate();
    //     memory[pte][2]=(d/10)+'0';
    //     memory[pte][3]=(d%10)+'0';
    //     va = (ir[2] - '0') * 10 + (ir[3] - '0');
    //     address();
    //     fault=false;
    //     pg++;
    // }
            addrs=map((ir[2]-'0')*10+ir[3]-'0');
            ra=addrs;
            for (int j = 0; j < 4; j++)
            {
                memory[ra][j] = r[j];
            }
        }
        else if (ir[0] == 'C' && ir[1] == 'R')
         { //  valid=false;
        //     va = (ir[2] - '0') * 10 + (ir[3] - '0');
        //     address();
        addrs=map((ir[2]-'0')*10+ir[3]-'0');
            ra=addrs;
            printf("ra=%d\n",ra);
            simulation();
            int cnt = 0;
            for (int j = 0; j < 4; j++)
            {
                if (memory[ra][j] == r[j])
                {
                    ++cnt;
                }
            }
            if (cnt == 4)
            {
                c = true;
            }
            else
            {
                c = false;
            }
        }
        else if (ir[0] == 'B' && ir[1] == 'T')
        {   simulation();
     
            int i = (ir[2] - '0') * 10 + (ir[3] - '0');
            if (c == true)
            {
                ic = i;
            }
        }
        else if((ir[0] != 'B' && ir[1] != 'T' )|| (ir[0] != 'C' && ir[1] != 'R') || (ir[0] != 'S' && ir[1] != 'R' )|| (ir[0] != 'L' && ir[1] != 'R') ||( ir[0] != 'P' && ir[1] != 'D') || (ir[0] != 'G' && ir[1] != 'D')|| (ir[0] != 'H')){
                pi=1;
                mos();
        } 
        //printf("error=%d ic=%d",error,ic);
        if(error==true){
         return;
    }
    //  for (int p = 0; p < 300; p++)
    // {
    //     printf("M[%d]", p);
    //     for (int q = 0; q < 4; q++)
    //     {
    //         printf("%c ", memory[p][q]);
    //     }
    //     printf("\n");
    // }
    //}
         
    }
}
void strt()
{
    for (int i = 0; i < 300; i++)
    {
        for (int j = 0; j < 4; j++)
        {
            memory[i][j] = '\0';
        }
    }
    for (int i = 0; i < 4; i++)
    {
        ir[i] = '\0';
        r[i] = '\0';
    }
    for (int i = 0; i < 40; i++)
    {
        buffer[i] = '\0';
    }
     for(int i=0;i<30;i++){
        key[i]='\0';
        value[i]='\0';
     }

    ic=0,c=false,si=0,pi=0,ti=0,ptr=-1,pte=-1,pg=0,kind=0;
}

void initialpcb(){

     pcb.jobid=((buffer[4]-'0')*1000+(buffer[5]-'0')*100+(buffer[6]-'0')*10+(buffer[7]-'0'));
     pcb.ttl=(buffer[8]-'0')*1000+(buffer[9]-'0')*100+(buffer[10]-'0')*10+(buffer[11]-'0');
     pcb.tll=(buffer[12]-'0')*1000+(buffer[13]-'0')*100+(buffer[14]-'0')*10+(buffer[15]-'0');
     pcb.llc=0;
     pcb.ttc=0;
     return;
}

void startexecution()
{
   // printf("starting program exe\n");
    ic = 0;
    executeprogram();
    if(error==true){
         return;
    }
}

void load()
{
    //inputf = fopen("in.txt", "r");
    if (inputf == NULL)
    {
        printf("file does'nt exist");
        return;
    }

    while (fgets(buffer, 41, inputf) != NULL)
    {
        //int p = 0;
        //     while(p<40 && buf2[p]!='\0'){
        //     buffer[p]=buf2[p];
        //     p++;
        // }
        if(buffer[0]=='\n'){
            continue;
        }
        // for (int v = 0; v < 40; v++)
        // {
        //     printf("B[%d]=%c\n", v, buffer[v]);
        // }
  

        if (buffer[0] == '$' && buffer[1] == 'A' && buffer[2] == 'M' && buffer[3] == 'J')
        {
            printf("amj part\n");
            initialpcb();
            strt();
            ptr=allocate()*10;
            inipgtable();
//              printf("jobid=%d ",pcb.jobid);
// printf("ttc=%d ",pcb.ttc);
// printf("ttl=%d ",pcb.ttl);
// printf("llc=%d ",pcb.llc);
// printf("tll=%d ",pcb.tll);
        }

        else if (buffer[0] == '$' && buffer[1] == 'D' && buffer[2] == 'T' && buffer[3] == 'A')
        {
            //printf("dta part\n");
            clearBuffer();
            startexecution();
        }
        else if (buffer[0] == '$' && buffer[1] == 'E' && buffer[2] == 'N' && buffer[3] == 'D')
        {
    //         for (int p = 0; p < 300; p++)
    // {
    //     printf("M[%d]", p);
    //     for (int q = 0; q < 4; q++)
    //     {
    //         printf("%c ", memory[p][q]);
    //     }
    //     printf("\n");
    // }
           strt();

            printf("end");
        }
        else
        {
            int k = 0;
            int d=allocate();
            //printf("d=%d\n",d);
            //printf("reached else part\n");
        
            memory[ptr+pg%10][2]=(char)((d/10)+48);
            memory[ptr+pg%10][3]=(char)((d%10)+48);
            
            pg++;
            ic=d*10;
            while (k < 40 && buffer[k] != '\0' && buffer[k]!='\n')
            {
                for (int j = 0; j < 4; j++)
                {
                    if (buffer[k] == 'H' && buffer[k]!='\n' )
                    {
                        memory[ic][j] = 'H';
                        ++k;
                        break;
                    }
                    memory[ic][j] = buffer[k];
                    ++k;
                }
                ++ic;
            }
        
        }
        if(error==true){
            //printf("reached error part\n");
         while(!(buffer[0] == '$' && buffer[1] == 'E' && buffer[2] == 'N' && buffer[3] == 'D')){
            for (int v = 0; v < 40; v++)
        {
            printf("B[%d]=%c\n", v, buffer[v]);
        }
            clearBuffer();
            fgets(buffer, 41, inputf);
         }
         error=false;
    }
    // for (int p = 0; p < 300; p++)
    // {
    //     printf("M[%d]", p);
    //     for (int q = 0; q < 4; q++)
    //     {
    //         printf("%c ", memory[p][q]);
    //     }
    //     printf("\n");
    // }
    clearBuffer();
    }


    for (int p = 0; p < 300; p++)
    {
        printf("M[%d]", p);
        for (int q = 0; q < 4; q++)
        {
            printf("%c ", memory[p][q]);
        }
        printf("\n");
   }
   
printf("value\n");
           for(int i=0;i<30;i++){
            printf("%d\n",value[i]);
           }
           printf("key\n");
           for(int i=0;i<30;i++){
            printf("%d\n",key[i]);
           }

  

  fclose(inputf);
}
int main()
{
    inputf=fopen("in2.txt","r");
    strt();
    load();
    //fclose(inputf);
    return 0;
}