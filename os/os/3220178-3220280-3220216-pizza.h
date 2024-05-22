//
//  3220178-3220280-3220216-pizza.h
//  os
//
//  Created by Rigas Sassalos, Anais Farhat, Alexandra Tsitsopoulou.
//

#ifndef _3220178_3220280_3220216_pizza_h
#define _3220178_3220280_3220216_pizza_h

#include <pthread.h>
#include <time.h>

//input constants
#define Ntel 2
#define Ncook 2
#define Noven 10
#define Ndeliverer 10

#define Torderlow 1
#define Torderhigh 5
#define Norderlow 1
#define Norderhigh 5

#define Pm 0.35
#define Pp 0.25
#define Ps 0.40

#define Tpaymentlow 1
#define Tpaymenthigh 3
#define Pfail 0.05

#define Cm 10
#define Cp 11
#define Cs 12

#define Tprep 1
#define Tbake 10
#define Tpack 1
#define Tdellow 5
#define Tdelhigh 15

typedef struct {
    int type; // 0: margherita, 1: peperoni, 2: special
    int count;
} PizzaOrder;

typedef struct {
    int id;
    int num_pizzas;
    PizzaOrder pizzas[Norderhigh];
    int total_price;
    struct timespec order_start_time;
} Order;

typedef struct {
    int revenue;
    int sales[3];
    int successful_orders;
    int failed_orders;
    int max_service_time;
    double total_service_time;
    int max_cold_time;
    double total_cold_time;
    pthread_mutex_t revenue_mutex;
    pthread_mutex_t sales_mutex;
    pthread_mutex_t operator_mutex[Ntel];
    pthread_mutex_t cook_mutex[Ncook];
    pthread_mutex_t oven_mutex[Noven];
    pthread_mutex_t deliverer_mutex[Ndeliverer];
} PizzaShop;

void init_shop();
void* customer_call(void* arg);

#endif /* _3220178_3220280_3220216_pizza_h */
