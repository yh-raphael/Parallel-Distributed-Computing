#ifndef __QUEUE_H__
#define __QUEUE_H__

// 큐 - 연결리스트 이용
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_MSG_LEN 1024

typedef struct _Request
{
    char msg[MAX_MSG_LEN];
    int fd;
} Request;

typedef struct _Node     // 노드 정의
{
    Request data;
    struct _Node *next;
} Node;

typedef struct _Queue    // Queue 구조체 정의
{
    Node *front;        // 맨 앞(꺼낼 위치)
    Node *rear;         // 맨 뒤(보관할 위치)
    int count;          // 보관 개수
} Queue;


void InitQueue(Queue *queue);               // 큐 초기화
int IsEmpty(Queue *queue);                  // 큐가 비었는지 확인
void Enqueue(Queue *queue, Request data);   // 큐에 삽입
Request Dequeue(Queue *queue);              // 큐에서 꺼냄

#endif