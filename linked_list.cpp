#include<stdio.h>
#include<malloc.h>
#include<iostream>
using namespace std;
struct node 
{
 int fd;
 string request;
 struct node *next;
};
typedef struct node node;

class linked_list
{
public:
node *head;
node * tail;
linked_list()
{
 head=NULL;
 tail=NULL;
}

void add_job(int fd, string str)
{

node *nd = (node*)malloc(sizeof(node));
nd->fd=fd;
nd->request=str;
	if(head==NULL)
	{
  
     head=nd;
     tail=nd;
	}
	else
	{

		tail->next=nd;
		tail=tail->next;
		tail->next=NULL;
	}
}

void del_job()
{

	if(head==tail)
	{
		free(head);
		head=NULL;
		tail=NULL;
	}
	
	else
	{
 		node * temp;
 		temp=head;
 		head=head->next;
 		free(temp);

	}
}

};