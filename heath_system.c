#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <time.h>

#define MAX 50
#define USERS_FILE "users.txt"
#define RECORDS_FILE "health_records.csv"
#define TEMP_FILE "temp.csv"
#define REPORTS_DIR "reports/"

#ifdef _WIN32
    #include <direct.h>   // for _mkdir
#else
    #include <sys/stat.h>
    #include <sys/types.h>
#endif

/* ================= PASSWORD HASH ================= */
uint32_t hashPassword(const char *p) {
    uint32_t h = 2166136261u;
    while (*p) { h ^= (unsigned char)*p++; h *= 16777619; }
    return h;
}

/* ================= UI HELPERS ================= */
void line() { printf("--------------------------------------------------\n"); }
void banner(const char *t) { line(); printf("  %s\n", t); line(); }

/* ================= AUTHENTICATION ================= */
int userExists(const char *u) {
    FILE *f = fopen(USERS_FILE, "r");
    if(!f) return 0;
    char user[MAX]; uint32_t h;
    while (fscanf(f, "%49s %u", user, &h) != EOF)
        if (!strcmp(user, u)) { fclose(f); return 1; }
    fclose(f); return 0;
}

void registerUser() {
    char u[MAX], p[MAX];
    banner("Register");
    printf("Username: "); scanf("%49s", u);
    if (userExists(u)) { printf("User exists!\n"); return; }
    printf("Password: "); scanf("%49s", p);
    FILE *f = fopen(USERS_FILE, "a");
    if(!f){ printf("Error opening users file!\n"); return; }
    fprintf(f, "%s %u\n", u, hashPassword(p));
    fclose(f);
    printf("Registration successful!\n");
}

int login(char *u) {
    char p[MAX], user[MAX]; uint32_t h, s;
    banner("Login");
    printf("Username: "); scanf("%49s", u);
    printf("Password: "); scanf("%49s", p);
    h = hashPassword(p);
    FILE *f = fopen(USERS_FILE, "r");
    if(!f){ printf("No users registered yet!\n"); return 0; }
    while (fscanf(f, "%49s %u", user, &s) != EOF)
        if (!strcmp(user, u) && h == s) { fclose(f); return 1; }
    fclose(f); printf("Invalid credentials!\n"); return 0;
}

/* ================= BMI LOGIC ================= */
const char* risk(float bmi) {
    if (bmi < 18.5) return "Moderate";
    if (bmi < 25) return "Low";
    if (bmi < 30) return "High";
    return "Critical";
}

void addBMI(const char *u) {
    int age; char g[10]; float w, h, bmi;
    banner("Add BMI Record");
    printf("Age: "); scanf("%d", &age);
    printf("Gender: "); scanf("%9s", g);
    printf("Weight(kg): "); scanf(" %f", &w);
    printf("Height(m): "); scanf(" %f", &h);

    if (w <= 0 || h <= 0) { 
        printf("Invalid weight or height!\n"); 
        return; 
    }

    bmi = w / (h * h);

    FILE *f = fopen(RECORDS_FILE, "a");
    if (!f) { printf("Error opening records file!\n"); return; }
    fprintf(f, "%s,%d,%s,%.2f,%.2f,%.2f,%s\n", u, age, g, w, h, bmi, risk(bmi));
    fclose(f);

    printf("BMI: %.2f | Risk: %s\n", bmi, risk(bmi));
}

/* ================= HISTORY / CRUD ================= */
void showHistory(const char *u) {
    FILE *f=fopen(RECORDS_FILE,"r");
    if(!f){ printf("No records found.\n"); return; }
    char l[200]; int i=1,found=0;
    banner("Your BMI History");
    while(fgets(l,200,f))
        if(strstr(l,u)){ printf("[%d] %s",i++,l); found=1;}
    if(!found) printf("No records.\n");
    fclose(f);
}

void deleteRecord(const char *u) {
    int id,c=0; char l[200];
    showHistory(u);
    printf("Enter record # to delete: "); scanf("%d",&id);
    FILE *in=fopen(RECORDS_FILE,"r");
    FILE *out=fopen(TEMP_FILE,"w");
    if(!in || !out){ printf("Error opening file!\n"); return; }
    while(fgets(l,200,in)){
        if(strstr(l,u)){ c++; if(c==id) continue; }
        fputs(l,out);
    }
    fclose(in); fclose(out);
    remove(RECORDS_FILE); rename(TEMP_FILE,RECORDS_FILE);
    printf("Record deleted successfully.\n");
}

void editRecord(const char *u) {
    int id,c=0,age; float w,h,bmi;
    char l[200],name[MAX],g[10],r[15];
    showHistory(u);
    printf("Enter record # to edit: "); scanf("%d",&id);
    FILE *in=fopen(RECORDS_FILE,"r");
    FILE *out=fopen(TEMP_FILE,"w");
    if(!in || !out){ printf("Error opening file!\n"); return; }
    while(fgets(l,200,in)){
        if(strstr(l,u)){
            c++;
            if(c==id){
                sscanf(l,"%[^,],%d,%[^,],%*f,%*f,%*f,%s", name,&age,g,r);
                printf("New weight: "); scanf(" %f",&w);
                printf("New height: "); scanf(" %f",&h);
                if (w <= 0 || h <= 0) { printf("Invalid input!\n"); fclose(in); fclose(out); return; }
                bmi=w/(h*h);
                fprintf(out,"%s,%d,%s,%.2f,%.2f,%.2f,%s\n", name,age,g,w,h,bmi,risk(bmi));
                continue;
            }
        }
        fputs(l,out);
    }
    fclose(in); fclose(out);
    remove(RECORDS_FILE); rename(TEMP_FILE,RECORDS_FILE);
    printf("Record updated successfully.\n");
}

/* ================= STATISTICS ================= */
void stats(const char *u){
    FILE *f=fopen(RECORDS_FILE,"r");
    if(!f){ printf("No records found.\n"); return; }
    char l[200]; float bmi,sum=0,min=1000,max=0; int c=0;
    while(fgets(l,200,f))
        if(strstr(l,u)){
            sscanf(l,"%*[^,],%*d,%*[^,],%*f,%*f,%f",&bmi);
            sum+=bmi; if(bmi<min)min=bmi; if(bmi>max)max=bmi; c++;
        }
    fclose(f);
    banner("BMI Statistics");
    if(!c){ printf("No data available.\n"); return; }
    printf("Records: %d\nAverage BMI: %.2f\nMin BMI: %.2f\nMax BMI: %.2f\n",
           c,sum/c,min,max);
}

/* ================= ASCII CHART ================= */
void chart(const char *u){
    FILE *f=fopen(RECORDS_FILE,"r");
    if(!f){ printf("No records found.\n"); return; }
    char l[200]; float bmi;
    banner("BMI Trend Chart");
    while(fgets(l,200,f))
        if(strstr(l,u)){
            sscanf(l,"%*[^,],%*d,%*[^,],%*f,%*f,%f",&bmi);
            printf("%5.1f | ",bmi);
            for(int i=0;i<(int)bmi;i++) printf("#");
            printf("\n");
        }
    fclose(f);
}

/* ================= EXPORT CSV ================= */
void exportCSV(const char *u){
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);
    char filename[100];
    snprintf(filename, sizeof(filename), REPORTS_DIR "report_%04d%02d%02d_%02d%02d%02d.csv",
             tm.tm_year+1900, tm.tm_mon+1, tm.tm_mday,
             tm.tm_hour, tm.tm_min, tm.tm_sec);

    // Create reports folder if not exists
    #ifdef _WIN32
        _mkdir(REPORTS_DIR);
    #else
        struct stat st = {0};
        if(stat(REPORTS_DIR,&st)==-1) mkdir(REPORTS_DIR,0777);
    #endif

    FILE *in=fopen(RECORDS_FILE,"r");
    if(!in){ printf("No records to export.\n"); return; }
    FILE *out=fopen(filename,"w");
    if(!out){ printf("Error creating report file!\n"); fclose(in); return; }

    char l[200];
    fprintf(out,"User,Age,Gender,Weight,Height,BMI,Risk\n");
    while(fgets(l,200,in))
        if(strstr(l,u)) fputs(l,out);
    fclose(in); fclose(out);
    printf("Exported to %s\n", filename);
}

/* ================= DASHBOARD ================= */
void dashboard(const char *u){
    int c;
    while(1){
        banner("Dashboard");
        printf("1 Add BMI Record\n2 View History\n3 Edit Record\n4 Delete Record\n");
        printf("5 Statistics Report\n6 BMI Trend Chart\n7 Export CSV\n8 Logout\nChoice: ");
        scanf("%d",&c);
        switch(c){
            case 1: addBMI(u); break;
            case 2: showHistory(u); break;
            case 3: editRecord(u); break;
            case 4: deleteRecord(u); break;
            case 5: stats(u); break;
            case 6: chart(u); break;
            case 7: exportCSV(u); break;
            case 8: return;
            default: printf("Invalid choice.\n");
        }
    }
}

/* ================= MAIN ================= */
int main(){
    int c; char u[MAX];
    while(1){
        banner("Health Monitoring System");
        printf("1 Register\n2 Login\n3 Exit\nChoice: ");
        scanf("%d",&c);
        if(c==1) registerUser();
        else if(c==2 && login(u)) dashboard(u);
        else if(c==3) break;
    }
    return 0;
}
