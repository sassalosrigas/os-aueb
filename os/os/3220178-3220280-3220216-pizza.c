//
//  3220178-3220280-3220216-pizza.c
//  os
//
//  Created by Rigas Sassalos, Anais Farhat, Alexandra Tsitsopoulou.
//

#include "3220178-3220280-3220216-pizza.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <pthread.h>

PizzaShop pizzaShop;

void init_shop() {
    pizzaShop.revenue = 0;
    for (int i = 0; i < 3; i++) {
        pizzaShop.sales[i] = 0;
    }
    pizzaShop.successful_orders = 0;
    pizzaShop.failed_orders = 0;
    pizzaShop.max_service_time = 0;
    pizzaShop.total_service_time = 0.0;
    pizzaShop.max_cold_time = 0;
    pizzaShop.total_cold_time = 0.0;
    pthread_mutex_init(&pizzaShop.revenue_mutex, NULL);
    pthread_mutex_init(&pizzaShop.sales_mutex, NULL);
    
    for (int i = 0; i < Ntel; i++) {
        pthread_mutex_init(&pizzaShop.operator_mutex[i], NULL);
    }
    for (int i = 0; i < Ncook; i++) {
        pthread_mutex_init(&pizzaShop.cook_mutex[i], NULL);
    }
    for (int i = 0; i < Noven; i++) {
        pthread_mutex_init(&pizzaShop.oven_mutex[i], NULL);
    }
    for (int i = 0; i < Ndeliverer; i++) {
        pthread_mutex_init(&pizzaShop.deliverer_mutex[i], NULL);
    }
}

int get_time_dif(struct timespec start, struct timespec end) {
    return (end.tv_sec - start.tv_sec) * 1000 + (end.tv_nsec - start.tv_nsec) / 1000000;
}

void* customer_call(void* arg) {
    int customer_id = *(int*)arg;
    struct timespec order_start_time;
    clock_gettime(CLOCK_REALTIME, &order_start_time);
    
    unsigned int seed = time(NULL) + customer_id;
    int order_time = rand_r(&seed) % (Torderhigh - Torderlow + 1) + Torderlow;
    sleep(order_time);
    
    int operator_id;
    for (operator_id = 0; operator_id < Ntel; operator_id++) {
        if (pthread_mutex_trylock(&pizzaShop.operator_mutex[operator_id]) == 0) {
            break;
        }
    }
    
    if (operator_id == Ntel) {
        for (operator_id = 0; operator_id < Ntel; operator_id++) {
            pthread_mutex_lock(&pizzaShop.operator_mutex[operator_id]);
            break;
        }
    }
    
    int num_pizzas = rand_r(&seed) % (Norderhigh - Norderlow + 1) + Norderlow;
    Order order;
    order.id = customer_id;
    order.num_pizzas = num_pizzas;
    order.total_price = 0;
    order.order_start_time = order_start_time;
    
    for (int i = 0; i < num_pizzas; i++) {
        float r = (float)rand_r(&seed) / RAND_MAX;
        if (r < Pm) {
            order.pizzas[i].type = 0;
            order.total_price += Cm;
        } else if (r < Pm + Pp) {
            order.pizzas[i].type = 1;
            order.total_price += Cp;
        } else {
            order.pizzas[i].type = 2;
            order.total_price += Cs;
        }
    }
    
    int payment_time = rand_r(&seed) % (Tpaymenthigh - Tpaymentlow + 1) + Tpaymentlow;
    sleep(payment_time);
    
    float payment_fail = (float)rand_r(&seed) / RAND_MAX;
    if (payment_fail < Pfail) {
        printf("Order with number %d failed.\n", customer_id);
        pthread_mutex_lock(&pizzaShop.revenue_mutex);
        pizzaShop.failed_orders++;
        pthread_mutex_unlock(&pizzaShop.revenue_mutex);
    } else {
        printf("Order with number %d submitted.\n", customer_id);
        pthread_mutex_lock(&pizzaShop.revenue_mutex);
        pizzaShop.revenue += order.total_price;
        pthread_mutex_unlock(&pizzaShop.revenue_mutex);
        
        for (int i = 0; i < num_pizzas; i++) {
            pthread_mutex_lock(&pizzaShop.sales_mutex);
            pizzaShop.sales[order.pizzas[i].type]++;
            pthread_mutex_unlock(&pizzaShop.sales_mutex);
        }
        
        int cook_id;
        for (cook_id = 0; cook_id < Ncook; cook_id++) {
            if (pthread_mutex_trylock(&pizzaShop.cook_mutex[cook_id]) == 0) {
                break;
            }
        }
        
        if (cook_id == Ncook) {
            for (cook_id = 0; cook_id < Ncook; cook_id++) {
                pthread_mutex_lock(&pizzaShop.cook_mutex[cook_id]);
                break;
            }
        }
        
        for (int i = 0; i < num_pizzas; i++) {
            sleep(Tprep);
        }
        
        struct timespec bake_start_time;
        clock_gettime(CLOCK_REALTIME, &bake_start_time);
        
        for (int i = 0; i < num_pizzas; i++) {
            pthread_mutex_lock(&pizzaShop.oven_mutex[i]);
        }
        
        sleep(Tbake);
        
        struct timespec bake_end_time;
        clock_gettime(CLOCK_REALTIME, &bake_end_time);
        
        for (int i = 0; i < num_pizzas; i++) {
            pthread_mutex_unlock(&pizzaShop.oven_mutex[i]);
        }
        
        pthread_mutex_unlock(&pizzaShop.cook_mutex[cook_id]);
        
        int deliverer_id;
        for (deliverer_id = 0; deliverer_id < Ndeliverer; deliverer_id++) {
            if (pthread_mutex_trylock(&pizzaShop.deliverer_mutex[deliverer_id]) == 0) {
                break;
            }
        }
        
        if (deliverer_id == Ndeliverer) {
            for (deliverer_id = 0; deliverer_id < Ndeliverer; deliverer_id++) {
                pthread_mutex_lock(&pizzaShop.deliverer_mutex[deliverer_id]);
                break;
            }
        }
        
        for (int i = 0; i < num_pizzas; i++) {
            sleep(Tpack);
        }
        
        struct timespec pack_end_time;
        clock_gettime(CLOCK_REALTIME, &pack_end_time);
        
        int delivery_time = rand_r(&seed) % (Tdelhigh - Tdellow + 1) + Tdellow;
        sleep(delivery_time);
        
        struct timespec delivery_end_time;
        clock_gettime(CLOCK_REALTIME, &delivery_end_time);
        
        int preparation_time = get_time_dif(order.order_start_time, pack_end_time) / 1000;
        printf("The order with the number %d was prepared in %d minutes\n", customer_id, preparation_time);
        
        int total_service_time = get_time_dif(order.order_start_time, delivery_end_time) / 1000;
        printf("The order with the number %d was delivered in %d minutes\n", customer_id, total_service_time);
        
        int cold_time = get_time_dif(bake_end_time, delivery_end_time) / 1000;
        
        pthread_mutex_lock(&pizzaShop.revenue_mutex);
        if (total_service_time > pizzaShop.max_service_time) {
            pizzaShop.max_service_time = total_service_time;
        }
        pizzaShop.total_service_time += total_service_time;
        if (cold_time > pizzaShop.max_cold_time) {
            pizzaShop.max_cold_time = cold_time;
        }
        pizzaShop.total_cold_time += cold_time;
        pthread_mutex_unlock(&pizzaShop.revenue_mutex);
        
        pthread_mutex_unlock(&pizzaShop.deliverer_mutex[deliverer_id]);
    }
    
    pthread_mutex_unlock(&pizzaShop.operator_mutex[operator_id]);
    return NULL;
}

int main(int argc, char* argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <number_of_customers> <random_seed>\n", argv[0]);
        return EXIT_FAILURE;
    }
    
    int Ncust = atoi(argv[1]);
    int seed = atoi(argv[2]);
    srand(seed);
    
    init_shop();
    
    pthread_t customers[Ncust];
    int customer_ids[Ncust];
    
    for (int i = 0; i < Ncust; i++) {
        customer_ids[i] = i + 1;
        pthread_create(&customers[i], NULL, customer_call, &customer_ids[i]);
    }
    
    for (int i = 0; i < Ncust; i++) {
        pthread_join(customers[i], NULL);
    }
    
    printf("Total revenue: %d euros\n", pizzaShop.revenue);
    printf("Sales: Margherita: %d, Peperoni: %d, Special: %d\n", pizzaShop.sales[0], pizzaShop.sales[1], pizzaShop.sales[2]);
    printf("Successful orders: %d\n", pizzaShop.successful_orders);
        printf("Failed orders: %d\n", pizzaShop.failed_orders);
        if (pizzaShop.successful_orders > 0) {
            printf("Average service time: %.2f minutes\n", pizzaShop.total_service_time / pizzaShop.successful_orders);
            printf("Max service time: %d minutes\n", pizzaShop.max_service_time);
            printf("Average cold time: %.2f minutes\n", pizzaShop.total_cold_time / pizzaShop.successful_orders);
            printf("Max cold time: %d minutes\n", pizzaShop.max_cold_time);
        }

    return EXIT_SUCCESS;
}


