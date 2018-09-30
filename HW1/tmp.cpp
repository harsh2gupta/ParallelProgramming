//
// Created by Harsh Gupta on 4/6/18.
//
#include <iostream>

using namespace std;

void sync(){
    cout <<"inside a";
    printf("%p\n", __builtin_extract_return_addr(__builtin_return_address(0)));
    void * addr = __builtin_extract_return_addr(__builtin_return_address(0));

    cout<<"sync end";
    return;

}
void main(){
    cout<<"start";
    sync();
    cout<<"after";
}