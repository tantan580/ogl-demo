#ifndef __gaussian__h
#define __gaussian__h
#include <iostream>
#include <vector>
#include <math.h>
#include <numeric>
//id from https://github.com/manuelbua/blur-ninja

int factorial(int num)
{
    if (num == 0)
        return 1;
    else
        return num * factorial(num - 1);
}

std::vector<int> & bionm(int row_index)
{
    //Computes the n-th coefficient or an entire row from Pascal's triangle
    //binomial coefficients.
    //"""
    std::vector<int> buf;
    buf.resize(row_index+1);

    for(int c = 0;c<row_index+1;c++)
    {
        buf[c] = factorial(row_index) / (factorial(row_index - c) * factorial(c));
    }
    return buf;
}

float binom(int row_index, int &column_index)
{
    //Computes the n-th coefficient or an entire row from Pascal's triangle
    //binomial coefficients.
    //"""
    return factorial(row_index)/(factorial(row_index - column_index) * factorial(column_index));
}

void kernel_binom(std::vector<float>&offsets,std::vector<float>&weights,int tabs = 5,int expand = 0,int reduce = 0)
{
    int row = tabs - 1 + (expand<<1);
    int coeffs_count = row + 1;
    int radius = tabs >>1;
    if (coeffs_count & 1 == 0)
    {
        std::cerr<<"Duped coefficients at center"<<std::endl;
    }
    //compute total weight
    //https://en.wikipedia.org/wiki/Power_of_two
    //ODO: seems to be not optimal ...
    int sumtotal = 0;
    for (int x = 0; x < reduce; ++x)
    {
        std::cout<<"x:"<<"binom(row,x)-->"<<binom(row,x)<<std::endl;
        sumtotal += binom(row,x)* 2 ;
    }
    float total = float(1<<row) - sumtotal;
//    std::cout<<"total:"<<total<<std::endl;
    //compute final weights
    for (int x = reduce+radius ; x>reduce-1 ; x--)
    {
        weights.emplace_back(binom(row,x)/total);
    }
    offsets.resize(radius+1);
    for (int x = 0 ;x<radius+1;++x)
    {
        offsets[x] = (float)x;
    }
}

void kernel_binom_linear(std::vector<float>&offsets,std::vector<float>&weights,int tabs = 5,int expand = 0,int reduce = 0)
{
    std::vector<float> bionm_offs,bionm_weis;
    kernel_binom(bionm_offs,bionm_weis,tabs,expand,reduce);
    int w_count = bionm_weis.size();
    int o_count = bionm_offs.size();
    if(w_count==0 || o_count ==0 )
    {
        std::cerr<<"Can't perform linear reduction pass, no input data"<<std::endl;
        return;
    }
    if(w_count & 1 ==0)
    {
        std::cerr<<"Duped coefficients at center"<<std::endl;
        return;
    }
    if((w_count -1)%2 != 0 )
    {
        std::cerr<<"Can't perform bilinear reduction on non-paired texels"<<std::endl;
        return;
    }
    weights.emplace_back(bionm_weis[0]);
    for (int x = 1; x < w_count -1 ; x +=2)
    {
        weights.emplace_back(bionm_weis[x] + bionm_weis[x+1]);
    }
    offsets.emplace_back(0.);
    for (int i = 0,x = 1;x < w_count - 1; x+=2,i++)
    {
        float val = (bionm_offs[x] * bionm_weis[x] + bionm_offs[x + 1] * bionm_weis[x + 1])/(weights[i + 1]);
        offsets.emplace_back(val);
    }
}



#endif
