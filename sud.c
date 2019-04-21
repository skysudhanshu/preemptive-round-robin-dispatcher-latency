#include<pthread.h>
#include<stdio.h>
#include<unistd.h>
#include<sys/wait.h>
#include<stdlib.h>
#define NEW 0
#define READY 1
#define WAITING 2
#define RUNNING 3
#define EXIT 4

int ongoing=0,runtime=0,ALLEXIT=0,TQ,do_ct=1,n;
struct Process_block 
{
	int pid;
	int state;
	int more_time_req;
        int priority;
	int at;
	int wt,tat,ct,exect;
	struct Process_block *prev;
	struct Process_block *next;
} *pa;
int chpp;
struct Queue
{
	struct Process_block *front ,*rear;
}*Ready;

void enqueue_p(struct Process_block *p)
{
	
	
	if(Ready->front==NULL)
	{
		Ready->front=p;
		Ready->rear=p;
		p->next=NULL;
	}
        
	else
	{
		if(p->priority>Ready->front->priority)
		{
			p->next=Ready->front;
			Ready->front->prev=p;
			Ready->front=p;
		}
               else if(p->priority==Ready->front->priority)
		{
			p->next=Ready->front->next;
			p->prev=Ready->front;
			Ready->front->next=p;
                         if(p->next!=NULL)
                      {
                        p->next->prev=p;
                      }
		}
		else if(p->priority<Ready->rear->priority)
		{
			p->next=NULL;
			Ready->rear->next=p;
			p->prev=Ready->rear;
			Ready->rear=p;
		}
		else
		{
                   struct Process_block *start=Ready->front->next;
		   while(start->priority>p->priority)
		   {
			   start=start->next;
		   }
                   if(start!=NULL&& p->priority==start->priority)
                   {
                          p->next=start->next;
                          start->next=p;
                          p->prev=start;
                      
                   }
                  else
               {
		   (start->prev)->next=p;
		   p->next=start;
		   p->prev=start->prev;
		   start->prev=p;
                }

		}
	}
}

void enqueue(struct Process_block *p)
{
       if(Ready->front==NULL)
      { 
         p->prev=NULL;

         p->next=NULL;
         Ready->front=p;           
         Ready->rear=p;
         return;
      }
     p->prev=Ready->rear;
     Ready->rear->next=p;
     Ready->rear=p;
}
struct Process_block *  deQueue()
{
     if(Ready->front==NULL)
     {
	     return NULL;
     }
     struct Process_block * temp=Ready->front;
     Ready->front=Ready->front->next;
     temp->next=NULL;
     if(Ready->front==NULL)
     {
	     Ready->rear=NULL;
     }
     return temp;
}
void checkqueue()
{
int count=0;
           for(int i=0;i<n;i++)
	   {
                   
		   if(pa[i].state == NEW && ongoing>=pa[i].at)
		   {      
			   
			   enqueue_p(&pa[i]);
			   pa[i].state=READY;
			   
                    }
                  if(pa[i].state==EXIT)
                  {
                    count++;
			}
	   }
	   if(count==n)
	   {
		  ALLEXIT=1; 
		  
	   }
}
int main()
{


Ready =(struct Queue*) malloc(sizeof(struct Queue));
	printf("Please enter No of processes to schedule ::");
	scanf("%d",&n);
	printf("\n  Please Enter Time qunatum ::");
	scanf("%d",&TQ);
	pa=(struct Process_block *)malloc(sizeof(struct Process_block)*n);
	for(int i=0;i<n;i++)
	{
             printf("\n\n Enter Process Id For Process %d ::",(i+1));
	     scanf("%d",&(pa[i].pid));
	     printf("\n Enter arrival time For Process %d ::",(i+1));
	     scanf("%d",&(pa[i].at));
	     printf("\n Enter Execution time For Process %d ::",(i+1));
	     scanf("%d",&(pa[i].more_time_req)); 
	     printf("\n Enter Priority for Process %d ::",(i+1));
             scanf("%d",&(pa[i].priority));
            pa[i].exect=pa[i].more_time_req;
	    pa[i].state=NEW;
	        
	}
 struct Process_block key; 
    int i,j;
    for (i = 1; i < n; i++) { 
        key = pa[i]; 
        j = i - 1; 
  
        while (j >= 0 && pa[j].at > key.at) { 
            pa[j + 1] = pa[j]; 
            j = j - 1; 
        } 
        pa[j + 1] = key; 
    } 
printf("\n Gannt Chart::\n");
printf("---------------------------------------------------------------------------------------------------------------------------------------\n");
struct Process_block *pr;
while(1)
{
        checkqueue();
        if(ALLEXIT==1)
        {
                     printf(" |%d|",ongoing);
                    break;      
        }
        
        if(Ready->front!=NULL && do_ct==1)
	{
             runtime=1;
             struct Process_block *prev;
             prev=pr;
	     pr=deQueue();
             pr->state=RUNNING;
             if(prev!=pr)
             {
                 printf("| [%d] | Process%d ",ongoing,pr->pid);
               }
             pr->more_time_req--;
	     ongoing++;
             if(runtime==pr->exect)
             {
                  do_ct=1;
                  pr->state=EXIT;
                  pr->ct=ongoing;
                  pr->tat=ongoing-pr->at;
                  pr->wt=pr->tat-pr->exect;
             }
             else
            {
               do_ct=0;
              }
	}
        else if(do_ct==0&&pr!=NULL && pr->state==RUNNING )
	{
             if(pr->more_time_req==0)
             {
                  do_ct=1;
                  pr->state=EXIT;
                  pr->ct=ongoing;
                  pr->tat=ongoing-(pr->at);
                  pr->wt=(pr->tat)-(pr->exect);
                  continue;
             }

             else if(runtime==TQ)
             {
                pr->state=READY;
                enqueue_p(pr);
                do_ct=1;
                continue;
              }

              else if(Ready->front !=NULL)
	     {
                    if(pr->priority<Ready->front->priority)
                    {
                      pr->state=READY;
                      enqueue(pr);
                      do_ct=1; 
                      continue; 
                    }
                    
             }      
             runtime++;
             pr->more_time_req--;
            ongoing++;

             if(pr->more_time_req==0)
             {
                  do_ct=1;
                  pr->state=EXIT;
                  pr->ct=ongoing;
                  pr->tat=ongoing-(pr->at);
                  pr->wt=(pr->tat)-(pr->exect);
             }            
             else if(runtime==TQ)
             {
                pr->state=READY;
                enqueue_p(pr);
                do_ct=1;
              }
             else if(Ready->front !=NULL)
	     {
                    if(pr->priority<Ready->front->priority)
                    {
                      pr->state=READY;
                      enqueue(pr);
                      do_ct=1;  
                    }
                    else
                    {
                         do_ct=0;
                    }
                   
             }
            else
          { 
              do_ct=0;
	  }

          
             
          
	}
else
{
printf("| [%d] | IDLE ",ongoing);
ongoing++;
}
}
printf("\n--------------------------------------------------------------------------------------------------------------------------------------");
int sumwt=0,sumtat=0;
for(int i=0;i<n;i++)
	{
           printf("\n\nprocess pid=%d\tct=%d\ttat=%d\twt=%d",pa[i].pid,pa[i].ct,pa[i].tat,pa[i].wt);
           sumwt+=pa[i].wt;
           sumtat+=pa[i].tat;
           
	}
printf("\n\n Avergae TAT=%f \n Average WT=%f\n\n",(sumtat/(n*1.0)),(sumwt/(n*1.0)));
}
