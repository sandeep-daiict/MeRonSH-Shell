//Author Sandeep Sharma 201205605 Mtech CSE IIIT-H
//Shell Implementation
#include<iostream>
#include<stdio.h>
#include<cstdlib>
#include<cstring>
#include<unistd.h>
#include<sys/wait.h>
#include<fcntl.h>
#include<fstream>


using namespace std;

char** tokenize(char*,char*);// Function to tokenize given string with delimiter
char* get_dir(string current);// Function to fetch just directory name not full path from curren directory...Used for shell prompt
int execute(char* command);//function that will execute command
void change_dir(char* );//function to change current directory
int parser(char*);//function for string parsing
void set_history(char*,char*);//function to set command to history.history file(.history) will be automatically created in user home folder.
char* search(char* s);//function to search pattern of bang(!) in history file
void set_variables();//function to set environment variables specified in .shellconf file in home folder
void read_variables();//function to set variable used in program by .shellrc file in home folder

int append_input=0;
int append_output=0;
int el=0;
char **str;
int num=0;
int total_pipes=0;
int set_tilde=0;
int background=0;
char* echo_env;
int MAX_SIZE=10; //Maximum size of command set in .shell conf file

string get_working_path();//Function to get current directory


//-----------------------------------------------------------------------------------------------------


int main()
{	
	

	set_variables();	//setting environment variables specified by user.
	read_variables();	//setting variables used in program
	char* token;
	char* dir;
	background=0;
	num=0;
	char *temp_dir;
	string cur_dir;
	
    /*
     char cwd[1024];
     if (getcwd(cwd, sizeof(cwd)) != NULL)
     fprintf(stdout, "Current working dir: %s\n", cwd);
     else
     perror("getcwd() error");
    */
	temp_dir=(char*)malloc(sizeof(char*)*MAX_SIZE);
	//prev_dir=get_current_dir_name();
    char prev_dir[1024];
    getcwd(prev_dir, sizeof(prev_dir));
	int status=0,check=0;
	pid_t pid_clear=fork();
	char* HOME=(char*)malloc(1);
	char* USER=(char*)malloc(1);
	
	if(pid_clear!=0)
	{
		wait(&status);
	}
	else
	{
		execvp("clear",NULL);   //to clear screen on running MesRonSH
		exit(0);
	}
	printf("-----------------------------------------Welcome to MesRonSH developed by Sandeep Sharma-------------------------------------------\n");
	printf("---------------------------------------------Enter exit for exiting MesRonSH------------------------------------------------\n");
	HOME=getenv("HOME");	//Getting Users Current Directory
	USER=getenv("USER");	//Getting Users name
	chdir(HOME);
	while(1)
	{	str=(char**)malloc(sizeof(char*)*100);
		for(el=0;el<100;el++)
		{
      		  	str[el]=(char*)malloc(sizeof(char)*MAX_SIZE);
		}
		echo_env=(char*)malloc(sizeof(char)*20);
		size_t size=MAX_SIZE;
		cur_dir=get_working_path();
        
        
        //Fetching current dirctory after every command
		dir=get_dir(cur_dir);
		char** env_var=(char**)malloc(1);
		char* command = (char*)malloc(sizeof(char)*MAX_SIZE);
		char* temp=(char*)malloc(sizeof(char)*MAX_SIZE);
		if(dir!=NULL && strcmp(cur_dir.c_str(),HOME)!=0)
		{
			cout<<USER<<"@MesRonSH:["<<dir<<"]$ ";//Printing Prompt
		}
		else if(strcmp(cur_dir.c_str(),HOME)==0)
		{
			cout<<USER<<"@MesRonSH:[~]$ ";      //Printing Prompt
		}
		else
		{
			cout<<USER<<"@MesRonSH:[/]$ ";    //Printing Prompt
		}
		
	
		getline(&command,&size,stdin);//Waiting For User INPUT ie command
		
		
		if(strcmp(command,"\n")==0)
		{
			continue;    // do not read new line(ENTER) and display prompt again
		}
		else
		{	
			command[strlen(command)-1]='\0';
			
		}
		if(strncmp(command,"cd\0",3)==0||strncmp(command,"cd \0",4)==0 || strncmp(command,"cd  \0",5)==0 || strncmp(command,"cd\t",3)==0 )
		{		
				change_dir(HOME);
				set_history(command,HOME);						//Change directory if only cd given to home 
				continue;
		}
		char* history_command=(char*)malloc(sizeof(char)*MAX_SIZE);
		if(*command=='!')
		{	
				
				 history_command=search(command); // bang (!) search for history and execute it
				
				strcpy(command,history_command);			
		}
		if(command[strlen(command)-1]=='&')
		{
			background=1;				// checking for Back Ground
			command[strlen(command)-1]='\0';
			
		}
		
		strcpy(temp,command);
		
		token=strtok(temp," ");
		if(strcmp(token,"pwd")==0 )
		{
			cout<<cur_dir<<"\n";           //pwd function....
		}
		else if(strcmp(token,"cd")==0 )         // Implementation of cd with ~,$ ,- or directory name
		{	
			
			token=strtok(NULL," ");
			
			if(strcmp(token,"-")!=0)        
			{
//				prev_dir=get_current_dir_name(); //portable method
                getcwd(prev_dir, sizeof(prev_dir));
							
			}
			
			
		
			if(strcmp(token,"~")==0)
			{	
				change_dir(HOME);	
			}
			if(strcmp(token,"-")==0)
			{	strcpy(temp_dir,prev_dir);
//				prev_dir=get_current_dir_name();
				getcwd(prev_dir, sizeof(prev_dir));
				change_dir(temp_dir);
				
				
			}
			else if(strcmp(token,"-")!=0 && strcmp(token,"~")!=0 && strcmp(token,"\0")!=0)
			{	if(token[0]=='$')
				{	token++;
					change_dir(getenv(token));	
				}
				else
				{
				change_dir(token);
				}
			}
			
			
		}
		
		else if(strcmp(token,"export")==0)     //Export command implementation
		{	token=strtok(NULL," ");
			env_var[0]=strtok(token,"=");
			env_var[1]=strtok(NULL,"=");
			
			
			setenv(env_var[0],env_var[1],1);
			
		}
		
		else if(strcmp(token,"exit")==0) // to exit command
			break;
		
		else if(strcmp(token,"history")==0) // history command
		{
			ifstream history_file;
			char* new_temp=(char*)malloc(sizeof(char)*MAX_SIZE);
			strcpy(new_temp,HOME);
			strcat(new_temp,"/.shell_history");
			
			history_file.open(new_temp,ios::app);
			char* hist_command=(char*)malloc(sizeof(char)*MAX_SIZE);
			int number=0;
			while ( history_file.getline(hist_command,MAX_SIZE) )
			{	number++;
				;
				printf("%d     %s\n",number,hist_command);
			}
			
			
			history_file.close();
			
		}
		else if(strcmp(token,"flush")==0)			//flush command to flush history
		{	char* new_temp=(char*)malloc(sizeof(char)*256);
			strcpy(new_temp,HOME);
			strcat(new_temp,"/.shell_history");
			remove(new_temp);				
		}
		else if(strcmp(token,"fg")==0)       // fg command to bring process to foreground
		{	int pid=0;
			//printf("Hello\n");
			token=strtok(NULL," ");
			pid=atoi(token);
			waitpid(pid,NULL,0);
		}	
		else
		{	
			check=execute(command); // executing external commands
			
			//continue;
		}
	set_history(command,HOME);
	
	free(command);
	free(temp);
			
	}
	printf("\tThanks for using MesRonSH\n\tYour Feedbacks are welcome!!!\n");
	return 0;
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------

void set_history(char* command,char* HOME)     //Function to set history
{
	ofstream history_file;
	char* new_temp=(char*)malloc(sizeof(char)*256);
	strcpy(new_temp,HOME);
	strcat(new_temp,"/.shell_history");
	
	history_file.open(new_temp,ios::app);
	history_file<<command;
	history_file<<"\n";
	history_file.close();	
}


//-------------------------------------------------------------------------------------------------------------------------------------------------------------

void change_dir(char* s) // function to change directory
{	//printf("yahoo\n");
	if(chdir(s)!=0)
	{
		printf("%s Directory not exist\n",s);
	}
	
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------


string get_working_path()
{
    char temp[1024];
    return ( getcwd(temp, 1024) ? std::string( temp ) : std::string("") );
}


//------------------------------------------------------------------------------------------------------------------------------------------------------------------

char* get_dir(string cur_dir)//function to fetch current directory
{	char* dup;
	char* token;
	char* next_token;
	next_token=(char*)malloc(sizeof(char)*MAX_SIZE);
	int i=0;
	dup=strdup(cur_dir.c_str());
        token=strtok(dup,"/");
	
	while(token != NULL)
	{	
		i++;
		strcpy(next_token,token);
		token=strtok(NULL,"/");
		
	
	}
	if(i!=0)
	{
		return next_token;
	}
	else
	{
		return token;
	}
}	


//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------


char** tokenize(char* str,char* c) // function for tokenizing string with given delimt
{
        char** result;
	char* token;
        int j,i=0;
        result=(char**)malloc(sizeof(char*)*MAX_SIZE);
        for(j=0;j<MAX_SIZE;j++)
        {
                result[j]=(char*)malloc(sizeof(char)*MAX_SIZE);
        }
        result[0]=strtok(str,c);
        token= result[0];
        while(token!=NULL)
        {       i++;
		if(strcmp(c,"|")==0)
		{	
		total_pipes++;
		}                
		token=strtok(NULL,c);
                result[i]=token;
                
        }
	num=i;
        return result;
}


//-----------------------------------------------------------------------------------------------------------------------------------------------------------


int execute(char* command) // function to execute external commands
{
	
		
	char** func;
	int i=0;
	
	int j=0;
	
	int **pipefd;
	int pipe_num=0;
			
	func = tokenize(command,"|"); // tokenizing with pipe
	pipe_num=total_pipes;
	total_pipes=0;
	

	pipefd = (int **) malloc (sizeof(int *) * (pipe_num  ) );   // creating pipe
    	for( int i = 0 ; i < (  pipe_num ) ; i++ )
	{
                 pipefd[i] = (int *) malloc( sizeof(int ) * 2);
                 pipe(pipefd[i]);
     	}
	
	i=0;	
	
	
	pid_t pid;
	
	int f_des_in,f_des_out;
	
	int pipe_executed=-1;
	int p=0,l=0;
	
	while(*func)
	{	
		pipe_executed++;
		
		for(i = 0; i <20; i++)
		{	for(j=0;j<MAX_SIZE;j++)
			{
        			str[i][j] = '\0';
     			}
		}
		append_input=0;
		append_output=0;
		set_tilde=0;
		
			
			parser(*func);
			
				pid=fork();    // forking to create seperate process for executing external commands
				
				
				if(pid==0)    // child process
				{	 
					if(pipe_num!=1)       // pipes implemented here
					{
					
            				 if(pipe_executed == 0)      // for first process
					 {	
						
						if(pipefd[pipe_executed][1]>2)       
						{
                 					dup2(pipefd[pipe_executed][1],1);
												
						}
						else
						{
							printf("OUTPUT PIPE CALL FAIL\n");
						}
					 }
           				else if (pipe_executed < pipe_num -1  )      // for Midlle processes
					{	      if(pipefd[pipe_executed][1]>2)
							{
            					     		dup2(pipefd[pipe_executed][1],1);
							}
							else
							{
								printf("output pipe call fail\n");
							}
							if(pipefd[pipe_executed-1][0]>2)
							{
          							dup2(pipefd[pipe_executed-1][0],0); 
							}
							else
							{
								printf("input pipe call fail\n");
							}
					}

				         else if( pipe_executed == (pipe_num-1 ) )// for last process
					 {	
							if(pipefd[pipe_executed-1][0]>2)
							{
	 				            		dup2(pipefd[pipe_executed-1][0],0);
							}
							else	
							{
								printf("INPUT PIPE CALL FAIL\n");
							}
					 }

           				 for(p=0;p<pipe_num;p++)
					 {
						for(l=0;l<pipe_num;l++)				// closing all open pipes
						{
							if(pipefd[p][l]>2)
							{
								close(pipefd[p][l]);
							}							
							
						}
					 }  
         		       
					}
				      	char* heredoc=(char*)malloc(sizeof(char)*256);
					char** argv;
					char s[MAX_SIZE];
					strcpy(s,"");
					
					argv=tokenize(str[3]," ");
					if(set_tilde)
					{						//handling Tilde
						argv[num]=getenv("HOME");	
					}

					if(strlen(echo_env)!=0)
					{
						argv[num]=getenv(echo_env);    // handling echo $environment variables
					}

					int newline=0;
					if(strlen(str[1])!=0)       // Input Redirections handled here str[1] has input
					{	
						if(append_input)
						{				// implementation of heredoc(<<) .heredoc file in home
							
							strcpy(heredoc,getenv("HOME"));
							strcat(heredoc,"/.shell_heredoc");
							
							f_des_in  = open (heredoc, O_RDWR | O_CREAT | O_APPEND, S_IRWXU);
							
							while(strcmp(s,str[1])!=0)
							{	printf("> ");
																			
								write (f_des_in , s, strlen(s));
								if(newline!=0)
									write (f_des_in , "\n", strlen(s));
								cin.getline(s,MAX_SIZE);
								newline++;
							}
							
							f_des_in = open (heredoc, O_RDWR);
							dup2(f_des_in,0);						
							
						}
						else				//implementation of input redirection (<)
						{     
							f_des_in = open (str[1], O_RDWR);	//opening input file in readonly mode
	      						if (f_des_in == -1)	//checking for input file opening error
							{
		  						printf ("Error opening file %s May be File exist\n",str[1]);
								
							}
							dup2(f_des_in,0);
														
						}
					
					}

					if(strlen(str[2])!=0)             //implementation of input redirection str[2] has redirection
					{							
						if(append_output)				//implementation of output redirection in append mode(>>)
						{       
							f_des_out = open (str[2], O_RDWR | O_CREAT | O_APPEND, S_IRUSR | S_IWUSR);	//creating output file
		 					if (f_des_out == -1)
		    					{
		      						printf ("Error creating output file \n may be storage full\n");
		    					}
						}
						else
						{	remove(str[2]);				//implementation of output redirection in append mode(>)
							f_des_out = open (str[2], O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);    //creating output file
		                                        if (f_des_out == -1)
		                                        {
		                                                printf ("Error creating output file \n may be storage full\n");
		                                        }

						}
					
						dup2(f_des_out,1);
						
					}
				
					
					if(execvp(str[0],argv)<0) // Executing external commands
					{
						printf("%s:Command do not exist\n",str[0]);
					}
					if(append_input)
					{
						remove(heredoc);			// remove temporary .heredoc file in home
					}	
					close(f_des_in);  //closing open descriptors
					close(f_des_out); 
		                	exit(1);
			
				}
			func++;
			
	}
						
					
					for(p=0;p<pipe_num;p++) // parent
					 {
						for(l=0;l<pipe_num;l++)
						{
							if(pipefd[p][l]>2)
							{
								close(pipefd[p][l]); // closing all open pipes in parent
							}							
							
						}
					 }  
					if(background==1)
					{				// For background process
						background=0;
						printf("Process ID : %d\n",pid);  // Displaying PID's of background process
						return 1;
					}
					
					for(i=0;i<pipe_num+1;i++)
						waitpid(pid,NULL,0);     // wait for child if no background mode
					
				
	num=0;
	return 1;
}



//----------------------------------------------------------------------------------------------------------------------------------------------------------

int parser(char* func)                            //Parser fuction for parsing string
{
	char* temp;
	int iter=0;
	int stream=0;
	int flag=0;
	
	temp=(char*)malloc(sizeof(char)*MAX_SIZE);
	strcpy(temp,func);
	int length=0;
	length=strlen(temp);

	int i=0;
	int flag_stream=0;
	int start_in=-1;
	int end_in=-1;
	int start_out=-1;
	int end_out=-1;
	int j=0;
	int start_echo=-1;
	int end_echo=-1;
	for(iter=0;iter<length;iter++)         //for extracting command from string
	{	
		if(temp[iter]==' ')
		{
			
			if(flag==1)
				break;
			continue;		
		}
		else
		{
			str[stream][i]=temp[iter];
			i++;	
			flag=1;	
		}
	
	}
	flag=0;
	
	for(iter=0;iter<length;iter++)                      //extracting input redirection string
	{	if(temp[iter]==' ' || temp[iter]=='>')
		{
			
			if(flag==1)
				break;
			continue;		
		}
		if(temp[iter]=='<')
		{	
			flag_stream=1;
			if(temp[iter+1]=='<')
			{
				append_input=1;		
				iter++;			
			}
			start_in=iter;				
			stream=1;
			i=0;
			continue;		
		}
		

		if(flag_stream==1)
		{
			str[stream][i]=temp[iter];
			i++;	
			flag=1;	
			end_in=iter;
		}
	
	}
	
	flag=0;
	flag_stream=0;
	for(iter=0;iter<length;iter++)   //extracting output redirection string
	{	if(temp[iter]==' ' || temp[iter]=='<')
		{
			
			if(flag==1)
				break;
			continue;		
		}
		
		if(temp[iter]=='>')
		{	
			flag_stream=1;
			if(temp[iter+1]=='>')
			{
				append_output=1;		
				iter++;
			}
			start_out=iter;
			stream=2;
			i=0;
			continue;		
		}

		if(flag_stream==1)
		{
			str[stream][i]=temp[iter];
			i++;	
			flag=1;	
			end_out=iter;
		}
	
	}
	j=0;
	int set_echo_flag=0; 
	for(iter=0;iter<length;iter++)   //extracting environment variables
	{	if(temp[iter]==' ')
		{
			if(set_echo_flag)
				break;
			continue;
		}
		if(temp[iter]=='$')
		{	start_echo=iter;
			set_echo_flag=1;	
		}
		else if(set_echo_flag==1)
		{
			echo_env[j]=temp[iter];
			j++;
			end_echo=iter;
		}
		
	}

	i=0;
	for(iter=0;iter<length;iter++)  // extracting arguments for internal commands
	{	if(temp[iter]=='~' )
		{	
			set_tilde=1;
			continue;		
		}
		if(temp[iter]=='<' || temp[iter]=='>')
			continue;
		if(start_echo<=iter && iter<=end_echo)
			continue;
		if(start_in<=iter && iter<=end_in)
			continue;
		if(start_out<=iter && iter<=end_out)		
			continue;
		str[3][i]=temp[iter];
		i++;
	}

	return 1;
}



//----------------------------------------------------------------------------------------------------------------------------------------------
char* search(char* s)     // Function for searching in history(!)
{	s++;
	char* history_search=(char*)malloc(sizeof(char)*256);
	ifstream history_file;
	char* str =(char*)malloc(sizeof(char)*256);	
	strcpy(history_search,getenv("HOME"));
	strcat(history_search,"/.shell_history");
	history_file.open(history_search,ios::out);
	int i=0;
	char* temp=(char*)malloc(sizeof(char)*256);
	while ( history_file.getline(str,256) )
	{
		if(strncmp(str,s,strlen(s))==0)
		{	
			strcpy(temp,str);
			
			
		}
	i++;
	}

	return temp;
}


//-------------------------------------------------------------------------------------------------------------------------------------------------------
void set_variables()    // Implementation of set variables fuction that set environment variables
{	
	char* env_variables=(char*)malloc(sizeof(char)*100);
	ifstream file;
	char* shellconf=(char*)malloc(sizeof(char)*256);
	
	char **var=(char**)malloc(1);
	strcpy(shellconf,getenv("HOME"));
	strcat(shellconf,"/.shellconf");
	file.open(shellconf,ios::out);
	
	while ( file.getline(env_variables,256) )
	{
		
		
		env_variables[strlen(env_variables)]='\0';		
		var=tokenize(env_variables,"=");
		
		setenv(var[0],var[1],1);

	}
	file.close();
}
//-------------------------------------------------------------------------------------------------------------------------------------------------------

void read_variables()   // Implementation of read variables fuction that set environment of shell
{	
	char* env_variables=(char*)malloc(sizeof(char)*100);
	ifstream file;
	char* shellrc=(char*)malloc(sizeof(char)*256);
	char **var=(char**)malloc(1);
	strcpy(shellrc,getenv("HOME"));
	strcat(shellrc,"/.shellrc");
	file.open(shellrc,ios::out);
	while ( file.getline(env_variables,256) )
	{
		
		
		env_variables[strlen(env_variables)]='\0';		
		var=tokenize(env_variables,"=");
		
		if(strcmp(var[0],"MAX_SIZE")==0)
		{ 	
			MAX_SIZE=atoi(var[1]);
		}
		
	}
	file.close();
}

//--------------------------------------------------------------------------------------------------------------------------------------------
