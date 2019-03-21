#include "../includes/socket.h"
#define LENGTH 10

int clients[LENGTH] = {-1};

struct User{
    char username[100];
    char password[100];
};

struct To{
    int conn;
    int idx;
};

struct Message{
    int conn;
    char mess[100];
    struct To to;
    int updated;
    struct User user;
    pthread_mutex_t mu;
}message[LENGTH];

struct Connection_struct{
    int conn;
    int idx;
};

void *client_process_init(void* connection);
void *client_process_send(void* connection);
void *client_process_recv(void* connection);

int server_init();
int reset_message(int pos);

int search_clients(const char* username){
    for(size_t i = 0; i < LENGTH; i++){
        if(strcmp(message[i].user.username,username) == 0) return i;
    }
    return -1;
}
int search_clients_conn(int conn){
    for(size_t i = 0; i < LENGTH; i++){
        if(message[i].conn == conn) return i;
    }
    return -1;
}

int main(int argv, char** argc){

    if(server_init()){
        eprint("Unable to initilize users!\n");
        exit(1);
    }
    struct sockaddr_in serv;
    int connection;
    int fd = socket(AF_INET,SOCK_STREAM,0);
    
    serv.sin_family = AF_INET;
    serv.sin_port = htons(8096);
    serv.sin_addr.s_addr = INADDR_ANY;

    for(int i = 0; i < LENGTH; i++){
        message[i].to.conn = -1;
        message[i].to.idx = -1;
        message[i].conn = -1;
    }

    bind(fd, (struct sockaddr *)&serv, sizeof(serv));

    listen(fd,LENGTH);

    pthread_t tid[LENGTH];
    int j = 0;
    while((connection = accept(fd, (struct sockaddr *)NULL, NULL))) {
        pthread_create(&tid[j++],NULL,client_process_init,(void*)&connection);
    }
    for(int i = 0; i < LENGTH; i++){
        pthread_join(tid[i],NULL);
        close(clients[i]);
    }
    return 0;
}


void *client_process_init(void* connection){
    struct Connection_struct conn_struct = *(struct Connection_struct*)connection;

    pthread_t tid_send;
    pthread_t tid_recv;
    char user_to[100] = {0};
    char username[100] = {0};
    char password[100] = {0};
    char user[2048] = {0};

    recv(conn_struct.conn,user,2048,0);

    sscanf(user,"%s : %s : %s",username,password,user_to);
    trim(username);
    trim(password);
    trim(user_to);

    memset(user,0,2048);
    int pos = search_clients(username);
    conn_struct.idx = pos;

    if(!(pos != -1 || strcmp(message[pos].user.password,password) != 0)){
        write(conn_struct.conn,"Wrong Username and Password",28);
        close(conn_struct.conn);
        reset_message(pos);
        return NULL;
    }

    message[pos].conn = conn_struct.conn;
    int flag = 0;
    size_t i = 0;
    for(; i < LENGTH; i++){
        if(!strcmp(message[i].user.username,user_to)){
            message[pos].to.conn = message[i].conn;
            message[pos].to.idx = i;
            flag = 1;
            break;
        }
    }

    if(!flag){
        write(conn_struct.conn,"Wrong Sender username",22);
        close(conn_struct.conn);
        reset_message(pos);
        return NULL;
    }

    while((message[pos].to.conn = message[i].conn) == -1);

    pthread_create(&tid_send,NULL,client_process_send,(void*)&conn_struct);
    pthread_create(&tid_recv,NULL,client_process_recv,(void*)&conn_struct);

    pthread_join(tid_send, NULL);
    pthread_join(tid_recv, NULL);

    close(conn_struct.conn);
    reset_message(pos);
    return NULL;
}

void *client_process_send(void* connection){
    struct Connection_struct conn_struct = *(struct Connection_struct*)connection;
    int pos = conn_struct.idx;
    while (1) {
        while(message[pos].updated == 0);
        pthread_mutex_lock(&message[message[pos].to.idx].mu);
        char temp_msg[200];
        sprintf(temp_msg,"\x1b[1m%s\x1b[0m  : %s",message[message[pos].to.idx].user.username,message[pos].mess);
        write(message[pos].to.conn,temp_msg,strlen(temp_msg));
        message[pos].updated = 0;
        memset(message[pos].mess,0,100);
        pthread_mutex_unlock(&message[message[pos].to.idx].mu);
    }
    return NULL;
}

void *client_process_recv(void* connection){
    struct Connection_struct conn_struct = *(struct Connection_struct*)connection;
    int pos = conn_struct.idx;
    while (1) {
        int ret = read(message[pos].conn,message[pos].mess,100);
        pthread_mutex_lock(&message[message[pos].to.idx].mu);
        if(ret < 0){
            return NULL;
        }else if(ret > 0)       
            message[pos].updated = 1;
        else 
            memset(message[pos].mess,0,100);
        pthread_mutex_unlock(&message[message[pos].to.idx].mu);
    }
    return NULL;
}

int reset_message(int pos){
    if(pos < 0){
        for(size_t i = 0; i < LENGTH; i++){
            message[i].conn = -1;
            memset(message[i].mess,0,100);
            memset(message[i].user.password,0,100);
            memset(message[i].user.username,0,100);
            message[i].to.conn = - 1;
            message[i].to.idx = - 1;
            message[i].updated = 0;
            pthread_mutex_init(&message[i].mu,NULL);
        }
    }else{
        int i = pos % LENGTH;
        message[i].conn = -1;
        memset(message[i].mess,0,100);
        memset(message[i].user.password,0,100);
        memset(message[i].user.username,0,100);
        message[i].to.conn = - 1;
        message[i].to.idx = - 1;
        message[i].updated = 0;
        pthread_mutex_init(&message[i].mu,NULL);
    }
    return 1;
}

int server_init(){
    reset_message(-1);
    DIR* d;
    struct dirent *dir;

    d = opendir("./users");

    if(!d) return 1;
    int i = 0;
    while((dir = readdir(d))){
        if(dir->d_name[0] != '.'){
            char path[100];
            sprintf(path,"./users/%s",dir->d_name);
            int fd = open(path,O_RDONLY);
            char pass[100];
            if(read(fd,pass,100) <= 0){
                close(fd);
                return 0;
            }
            trim(pass);
            strcpy(message[i].user.username,dir->d_name);
            strcpy(message[i++].user.password,pass);
            close(fd);
        }
    }

    return 0;
}