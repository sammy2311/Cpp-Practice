#include<stdio.h>
#include<stdarg.h>
#include<unistd.h>
#include <stdlib.h>

int print_char(int c)
{
    return write(1,&c,1);
}

int print_str(char *str)
{
    int count = 0;
    while(*str != '\0'){
        print_char((int)*str);//changing *str to int as print_char takes int =>explicit typecast
        ++str;
        ++count;

    }
    return count;

}


int print_digit(long n, int base)
{
    int count = 0;
    char *symbols = "01234456789abcdef"; //All symbols to write a hexa decimal and a normal number 
    if(n < 0)
    {
        write(1,"-",1);//immidiately writes a - sign
        return print_digit(-n,base) + 1;//Shitty recursive call for handling -ve values
        //we write + 1 for incrementing the counter as we had to use 1 byte for the - sign

    }
    else if(n< base)
    {
        return print_char(symbols[n]);
    }
    else 
    {
        count = print_digit(n / base, base);
        return count + print_digit(n % base, base);
    }


}


int print_format(char specifier, va_list ap)
{
    int count = 0;
    if(specifier =='c')
        count += print_char(va_arg(ap,int ));//We write int here as a type promotion takes place
    else if(specifier =='s')
        count += print_str(va_arg(ap,char* ));
    else if(specifier=='d')
        count += print_digit((long)va_arg(ap,int),10);
    else if(specifier =='x')
        count += print_digit((long)va_arg(ap,unsigned int),16);
    else
        count += write(1, &specifier,1);

    return count;

}

//Need not call it format but we'll call it that as the strings are format strings
int my_printf(const char *format, ...)
{
    va_list ap;
    int cnt = 0;


    va_start(ap,format);
    while(*format != '\0')
    {
        if(*format == '%'){
            cnt += print_format(*++format, ap); //++ has higher precedence than * 
        }
        else
        {
            cnt += write(1,format,1);
        }
        ++format; 
    }
    va_end(ap);
    return cnt;
}



int main()
{
    int count;
    my_printf("Hello %s, today is %d and in hex %x, char->%c\n","Samarth", 21,21,'z');


    return 0;

}