#include <stdio.h>
#include <stdlib.h>
#include <string.h>
int ptrsize=sizeof(char*);


typedef struct database_Element{
    int ID;
    int field_Size;
    char* field_Name;
    char* field_Value;


}db_Element;

typedef struct Database{
    char* db_Name;
    db_Element* elements;
    int row_Size;
    int num_Of_Rows;
    int num_Of_Columns;
    int ID;

}db;

void newDb(db* dataBase){
    dataBase->num_Of_Rows=0;
    FILE* fp;
    int num,i,x;
    int id=1,field_size;
    char* field_name;
    printf("Enter database name without .db (max 20 characters).\n");
    char* name=(char*)malloc(24);
    scanf("%s",name);
    dataBase->db_Name=name;

    printf("How many columns your database will have?\n");
    scanf("%d",&num);
    db_Element *elem=(db_Element*)malloc(sizeof(db_Element)*num);
    dataBase->elements=elem;
    //field_name=(char*)malloc(21);

    for(i=0;i<num;i++){
        printf("Enter column name(max 20 characters)\n");
        field_name=(char*)malloc(21);
        scanf("%s",field_name);
        elem[i].field_Name=field_name;
        printf("Enter field size.\n");
        scanf("%d",&field_size);
        elem[i].field_Size=field_size;
        elem[i].ID=1;
        /*field_value=(char*)malloc(field_size);
        elem[i].field_Value=field_value;*/

    }
    x=strlen(name);
    name[x]='.';
    name[x+1]='d';
    name[x+2]='b';
    name[x+3]='\0';
    fp=fopen(name,"wb");
    int format=16711935,rows=0;
    fwrite(&format,4,1,fp);
    fwrite(name,24,1,fp);
    fwrite(&num,4,1,fp);
    fwrite(&rows,4,1,fp);
    fwrite(&id,4,1,fp);
    for(i=0;i<num;i++){
        char a[21];
        int b;
        char c='\0';
        memmove(a,elem[i].field_Name,21);
        b=elem[i].field_Size;
        fwrite(&id,4,1,fp);
        fwrite(a,21,1,fp);
        fwrite(&b,4,1,fp);
        fwrite(&c,1,elem[i].field_Size,fp);

    }
    fclose(fp);
    free(name);
    for(i=0;i<num;i++){
        free(elem[i].field_Name);

    }
    free(elem);
    /*fp=fopen(name,"rb");
    rewind(fp);
    fseek(fp,73,SEEK_CUR);
    char buff[30];
    fread(buff,30,1,fp);
    printf("\n%s",buff);*/





}

FILE* readDbHeader(db* dataBase,char fileName[]){
    FILE* fp=fopen(fileName,"rb");
    char name[24];
    int format,num_of_columns,num_of_rows,id;
    fread(&format,4,1,fp);
    if(format==16711935){
        printf("File opened.\n");
    }
    else{
        printf("Unknown file format!\n");
        return 0;
    }

    fread(name,24,1,fp);
    printf("Database name: %s\n",name);
    fread(&num_of_columns,4,1,fp);
    printf("Number of columns: %d\n",num_of_columns);
    fread(&num_of_rows,4,1,fp);
    printf("Number of rows: %d\n\n",num_of_rows);
    fread(&id,4,1,fp);
    dataBase->num_Of_Rows=num_of_rows;
    dataBase->num_Of_Columns=num_of_columns;
    dataBase->ID=id;

    return fp;


}
void readRow(db* dataBase,char fileName[]){
    dataBase->row_Size=0;
    char *a;
    int b,id;
    char *c;
    FILE* fp=readDbHeader(dataBase,fileName);
    int i,r=dataBase->num_Of_Columns;
    db_Element* elem=(db_Element*)malloc(r*sizeof(db_Element));
    dataBase->elements=elem;
    for(i=0;i<r;i++){
        a=(char*)malloc(21);
      fread(&id,4,1,fp);
      fread(a,21,1,fp);
      fread(&b,4,1,fp);
      c=(char*)malloc(b);
      fread(c,b,1,fp);
      elem[i].field_Name=a;
      elem[i].field_Size=b;
      elem[i].field_Value=c;
      dataBase->row_Size+=(b+29);

    }
    fclose(fp);


}

void printTitle(db* dataBase){
    char name[25];
    int r=dataBase->num_Of_Columns,j,k;
    //for(i=0;i<r;i++){
    //memmove(name,dataBase->elements[i].field_Name,25);
       for(j=0;j<(9+22*r);j++){
            printf("-");
       }
       printf("\n");
       printf("| ID    |");
       for(j=0;j<r;j++){
            memmove(name,dataBase->elements[j].field_Name,21);
            printf(" %s",name);

            int n=strlen(name);
            //printf("   %d   ",n);
            for(k=0;k<(20-n);k++){
                printf(" ");
            }
            printf("|");
       }
       printf("\n");
       for(j=0;j<(9+22*r);j++){
            printf("-");
            }

       printf("\n");
       for(j=0;j<(9+22*r);j++){
            printf("-");
       }
       printf("\n");
}

void printValues(db* dataBase,char fileName[]){
    if(dataBase->num_Of_Rows==0)
        return;
    int id,b,r=dataBase->num_Of_Columns,i,j,it=0;
    char value[256];
    FILE* fp=fopen(fileName,"rb");
    fseek(fp, 40+dataBase->row_Size, SEEK_CUR);
    while(1){
        fread(&id,4,1,fp);
        fseek(fp,-4,SEEK_CUR);
        int len=0,x=id;
        while(x){
            x/=10;
            len++;
        }
        printf("| %d ",id);
        for(i=0;i<(5-len);i++){
            printf(" ");
        }
        for(i=0;i<(dataBase->num_Of_Columns);i++){
        //fread(&id,4,1,fp);
        fseek(fp,4,SEEK_CUR);
        fseek(fp,21,SEEK_CUR);
        fread(&b,4,1,fp);
        fread(value,b,1,fp);
        printf("| %s ",value);
        for(j=0;j<(19-strlen(value));j++){
            printf(" ");
        }

        }
        printf("|");
        printf("\n");
        it++;
        if(it==dataBase->num_Of_Rows)
            break;
    }


}

void addRow(db* dataBase,char fileName[]){
    int i;
    dataBase->num_Of_Rows++;
    int row=dataBase->num_Of_Rows,id;
    FILE* fp=fopen(fileName,"a+b");
    for(i=0;i<dataBase->num_Of_Columns;i++){
        char a[21];
        int b;
        id=dataBase->ID;
        char *c;
        memmove(a,dataBase->elements[i].field_Name,21);
        b=dataBase->elements[i].field_Size;
        fwrite(&id,4,1,fp);
        fwrite(a,21,1,fp);
        fwrite(&b,4,1,fp);
        c=(char*)malloc(b);
        printf("Enter value for field with ID: %d and column name: %s. \(FIELD SIZE: %d\)",id,a,b);
        scanf("%s",c);
        fwrite(c,b,1,fp);


    }
    id++;
    dataBase->ID++;
    fclose(fp);
    fp=fopen(fileName,"r+b");
    fseek(fp,32,SEEK_CUR);
    fwrite(&row,4,1,fp);
    fwrite(&id,4,1,fp);
    fclose(fp);





}

void deleteData(db* dataBase,char fileName[]){
    char *buffer;
    int del,id,skip=0;
    readRow(dataBase,fileName);
    buffer=(char*)malloc(40+dataBase->row_Size);
    FILE* fp=fopen(fileName,"rb");
    FILE* fp2=fopen("tmp","wb");
    if(!fp){
        perror("Error!");
        return;
    }
    if(!fp2){
        perror("Error2!");
        return;
    }

    fread(buffer,40+dataBase->row_Size,1,fp);
    fwrite(buffer,40+dataBase->row_Size,1,fp2);
    fclose(fp2);
    fp2=fopen("tmp","a+b");
    if(!fp2)
        perror("Error2");

   // fseek(fp,36+dataBase->row_Size,SEEK_CUR);
   // fseek(fp2,36+dataBase->row_Size,SEEK_CUR);
   printf("Enter ID of the item you want to delete.\n");
    scanf("%d",&del);
    int it=0;
    while(1){
        fread(&id,4,1,fp);
        fseek(fp,-4,SEEK_CUR);
        if(del>dataBase->ID-1)
            return;
        if(id!=del){
            fread(buffer,dataBase->row_Size,1,fp);
            fwrite(buffer,dataBase->row_Size,1,fp2);
            //fseek(fp,(dataBase->row_Size),SEEK_CUR);
        }
        else{
            skip=1;
            //break;
            fseek(fp,dataBase->row_Size,SEEK_CUR);
            }
        it++;
        if(it==dataBase->num_Of_Rows){//id+1)==dataBase->ID
            break;

            }

    }
    if(skip==1){
        dataBase->num_Of_Rows--;
    }

   /* fseek(fp2,(dataBase->row_Size)*skip,SEEK_CUR);
    fseek(fp,dataBase->row_Size,SEEK_CUR);

    while(1){
        fread(buffer,(dataBase->row_Size),1,fp);
        fwrite(buffer,(dataBase->row_Size),1,fp2);
        if(feof(fp))
            break;
    }*/
    fclose(fp);
    fclose(fp2);

    int n=dataBase->row_Size;
    char *buff=(char*)malloc(n);

    fp=fopen(fileName,"wb");
    fp2=fopen("tmp","rb");
    while(n){
        n=fread(buff,1,n,fp2);
        fwrite(buff,n,1,fp);

    }
    int idv=dataBase->num_Of_Rows;
    fclose(fp);
    fclose(fp2);
    fp=fopen(fileName,"r+b");
    fseek(fp,32,SEEK_CUR);
    fwrite(&idv,4,1,fp);
    fclose(fp);
    fp=fopen("tmp","wb");
    fclose(fp);
    free(buffer);
    //printf(" %d %d ",ftell(fp),ftell(fp2));



}

void searchData(db* dataBase,char fileName[]){
    FILE* fp=fopen(fileName,"rb");
    int i,id,size,s=0,m=0;
    char *buffer;
    char* value;
    int* found=(int*)malloc(sizeof(int)*dataBase->num_Of_Columns);
   // char** values=(char**)malloc(sizeof(char*)*dataBase->num_Of_Columns);
    char* values[20];
    readRow(dataBase,fileName);
    for(i=0;i<dataBase->num_Of_Columns;i++){
        printf("Enter search value for field '%s' (Enter '*' to skip this parameter.)\n",dataBase->elements[i].field_Name);
        buffer=(char*)malloc(dataBase->elements[i].field_Size);
        scanf("%s",buffer);
        values[i]=buffer;

    }
    fseek(fp,40+dataBase->row_Size,SEEK_CUR);
    char* readValues[20];
    int j;
    printTitle(dataBase);
    while(1){
        for(i=0;i<dataBase->num_Of_Columns;i++){
            fread(&id,4,1,fp);
            fseek(fp,21,SEEK_CUR);
            fread(&size,4,1,fp);
            value=(char*)malloc(size);
            fread(value,size,1,fp);
            readValues[i]=value;
            if(strcmp("*",values[i])==0){
                found[i]=1;
            }
            else{
                if(strcmp(value,values[i])==0){
                    found[i]=1;
                }
                else{
                    found[i]=0;
                }
            }
            //free(value);
        }

        for(i=0;i<dataBase->num_Of_Columns;i++){
            if(found[i]==1){
                m++;
            }

        }

        if(m==dataBase->num_Of_Columns){
            printf("| %d ",id);
            int len=0,x=id;
            while(x){
                x/=10;
                len++;
            }
            for(i=0;i<(5-len);i++){
                printf(" ");
            }

            for(i=0;i<(dataBase->num_Of_Columns);i++){

                printf("| %s ",readValues[i]);
                for(j=0;j<(19-strlen(readValues[i]));j++){
                    printf(" ");
                    }

            }
            printf("|");
            printf("\n");





        }
        m=0;
        for(i=0;i<dataBase->num_Of_Columns;i++){
            found[i]=0;
        }
        for(i=0;i<dataBase->num_Of_Columns;i++){
            free(readValues[i]);
        }

        s++;
        if(s==dataBase->num_Of_Rows){
        break;

    }

    }

    fclose(fp);


}

int main()
{
    char fileName[FILENAME_MAX];
    int in;
    db* dataBase=(db*)malloc(sizeof(db));
    int input;
    printf("Enter 1 to open existing database\nEnter 2 to create new database\n");
    scanf("%d",&in);
    if(in==1){
        printf("\nEnter database name\(i.e. name.db\)\n");
        scanf("%s",fileName);

        while(1){
        printf("\nEnter 1 to add data to existing database\nEnter 2 to print existing database\nEnter 3 to delete data from database\nEnter 4 to search data\nEnter 5 to open another database\nEnter 6 to exit\n");
        scanf("%d",&input);
        if(input==1){
            char c;
            readRow(dataBase,fileName);
            while(1){
                addRow(dataBase,fileName);
                printf("\nAdd more data?");
                scanf(" %c",&c);
                if(c=='n' || c=='N')
                    break;
                else if(c!='y' && c!='Y'){
                    printf("Wrong input!\n");
                    break;
                    }

            }

        }
        else if(input==2){
            readRow(dataBase,fileName);
            printTitle(dataBase);
            printValues(dataBase,fileName);
        }
        else if(input==3){
            deleteData(dataBase,fileName);
        }
        else if(input==4){
            searchData(dataBase,fileName);
        }
        else if(input==5){
            printf("\nEnter database name\(i.e. name.db\)\n");
            scanf("%s",fileName);
        }
        else if(input==6){
            break;
        }
        else{
            printf("Wrong input!\n");
        }


    }
    }
    else if(in==2){
        newDb(dataBase);
    }
    else{
        printf("Wrong input!");
        return;
    }



   /* //newDb(dataBase);
    //readDbHeader(dataBase,"myDB.db");
    readRow(dataBase,"myDB.db");
    //addRow(dataBase);
    printTitle(dataBase);
    printValues(dataBase,"myDB.db");*/

    return 0;
    }
