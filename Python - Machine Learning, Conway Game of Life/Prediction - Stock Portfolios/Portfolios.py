"""
# Lior Ichiely
# Introduction to Computer Science - Simple stock prediction

The following is a class that grabs stocks data from the internet and calculates the universal portfolio and exponential portfolio programs.
"""

import numpy as np
import pandas as pd
import pandas_datareader.data as web
from datetime import date
import itertools as itr
import math
import yfinance
from typing import List

def portbuild(port:list, n:int) -> list:
    """
    Used to find all possible portfolios, given the set of possible ratings and amount of stocks.
    :param port: The list of all possible probabilities
    :param n: Amount of stocks
    :return: All possible portfolios
    """
    anest = list(itr.product(port, repeat=n))
    anestcopy = anest.copy()
    for i in range(len(anest)):
        if (sum(anestcopy[i]) > 1.001 or sum(anestcopy[i]) < 0.999):
            anest[i] = 'X'
    while 'X' in anest:
        anest.remove('X')
    return anest

class PortfolioBuilder:

    def __init__ (self):
        self.df = None

    def get_daily_data(self, tickers_list: List[str],start_date: date,end_date: date = date.today()) -> pd.DataFrame:
        """
        get stock tickers adj_close price for specified dates.

        :param List[str] tickers_list: stock tickers names as a list of strings.
        :param date start_date: first date for query
        :param date end_date: optional, last date for query, if not used assumes today
        :return: daily adjusted close price data as a pandas DataFrame
        :rtype: pd.DataFrame

        example call: get_daily_data(['GOOG', 'INTC', 'MSFT', ''AAPL'], date(2018, 12, 31), date(2019, 12, 31))
        """
        try: self.df = web.get_data_yahoo(tickers_list, start_date, end_date)
        except:
            raise ValueError
        #self.df = pd.DataFrame.drop(self.df, labels=["High", "Low", "Open", "Close", "Volume"], axis='columns')
        self.df = self.df['Adj Close']
        self.list = self.df.values.tolist() # DataFrame to List
        #self.tofloat64()
        self.prod = len(self.list[0]) # Number of stocks
        if(self.df.isnull().values.any()):
            raise ValueError # Check that the list is ok, raise error otherwise
        self.days = int(len(self.list)) # Data length in days
        # Empty vector = Empty vector as big as the amount of stocks. Useful for many of the calculations.
        self.emptyvector=[]
        for i in range(self.prod): self.emptyvector.append(0)

        return self.df

    def tofloat64(self):
        # Unused funciton, unnecessary
        for i in range(len(self.list)):
            for j in range(len(self.list[i])):
                self.list[i][j]=np.float64(self.list[i][j])

    def find_xt(self, day: int):
        """
        Finds the vector x(t).
        :param day: day of data
        :return: vector x(t)
        """
        #print(len(self.list))
        vector=self.list[day].copy()
        vectorm=self.list[day-1].copy()

        for i in range(len(vector)): # Find xt!
            #print(vector[i],vectorm[i])
            vector[i]=vector[i]/vectorm[i]
        return vector

    def wealth_product_constant(self, days: int, port: list): # How S_t(b) is calculated with a constant portfolio
        s0=1
        for i in range(1,days+1):
            #print(port,self.find_xt(i))
            s0*=np.dot(port,self.find_xt(i)) # DOT(b,x(t))
        return s0

    def wealth_productr_fluent(self, days: int, mul_port: list): # How S_t(b) is calculated according to earlier portfolios
        s0=1
        for i in range(0,days):
            #print(i)
            #print(mul_port)
            #print(days)
            s0*=np.dot(mul_port[i],self.find_xt(i+1)) # DOT(b(t),x(t))
            #print(s0)
        return s0

    # I could combine the two above functions by checking which is a matrix(list of lists), I chose to write it this way to simplify.

    def expcalc(self,pos: int,day: int, b: list, n: int): # -> Scalar
        """
        Used to calculate the exponential sum according to the formula in the assignment.
        :param pos: Position in vector - b(t)_j
        :param day: date t of calculation
        :param b: vector b(t)
        :param n: Learn rate n
        :return: The sum
        """
        vectorx=self.find_xt(day) # Find the x(t) vector at the specified date
        dot=np.dot(b,vectorx)
        total=b[pos]*np.exp((n*vectorx[pos])/dot) #Finds the b(t)_j for a SINGULAR position
        return total

    def find_universal_portfolio(self, portfolio_quantization: int = 20) -> List[float]:
        """
        Calculates the universal portfolio for the previously requested stocks

        :param int portfolio_quantization: size of discrete steps of between computed portfolios. each step has size 1/portfolio_quantization
        :return: returns a list of floats, representing the growth trading  per day
        """
        # Step 1: Find all possible portfolios.
        m = self.prod  # Amount of products
        a_int = portfolio_quantization # Jump Distance
        aprob = []
        for i in range(a_int + 1):
            aprob.append(i / a_int)
        # aprob now has all possible scalars. Use portbuild to create all possible portfolios.
        portfolios = portbuild(aprob, m)  # Portfolios
        # Now we have all the possible portfolios in "portfolios". Let us calculate the universal portfolio product.
        # Step 2: Generate a list of universal portfolios up to date t.
        # Step 3: Calculate the wealth according at day t+1 according to the list above.
        uni_list=[]
        wealth_list = []
        for i in range(0,self.days):
            vector=self.emptyvector.copy() # Reset to the empty vector, we will need it
            sum=0
            #
            for port in portfolios:
                sum+=self.wealth_product_constant(i,port)
                vector=np.add(vector,np.dot(port,self.wealth_product_constant(i,port)))
            #
            vector=np.dot(vector,1/sum)
            wealth_list.append(self.wealth_productr_fluent(len(uni_list), uni_list))
            uni_list.append(vector)
            #print(uni_list,len(uni_list))

        return wealth_list

    def find_exponential_gradient_portfolio(self, learn_rate: float = 0.5) -> List[float]:
        """
        calculates the exponential gradient portfolio for the previously requested stocks

        :param float learn_rate: the learning rate of the algorithm, defaults to 0.5
        :return: returns a list of floats, representing the growth trading  per day
        """
        wealth_list=[1.0]
        explist=[]
        b0=self.emptyvector.copy()
        for i in range(len(b0)): b0[i]=1/self.prod #Assume first day split equally.
        explist.append(b0)
        # Now generate list of b portfolios as a function of t.

        for i in range(1,self.days):
            #print(i)
            b=self.emptyvector.copy()
            for j in range(self.prod):
                #print(explist[i-1])
                counter=self.expcalc(j,i,explist[i-1],learn_rate)
                denominator=0
                for k in range(self.prod):
                    denominator+=self.expcalc(k,i,explist[i-1],learn_rate)
                b[j]=counter/denominator
            wealth_list.append(self.wealth_productr_fluent(len(explist), explist)) # Calculate the wealth so far
            explist.append(b)
            #print(self.wealth_productr_fluent(len(explist), explist))

        return wealth_list



if __name__ == '__main__':  # You should keep this line for our auto-grading code.
    #print('write your tests here')  # don't forget to indent your code here!
    pb=PortfolioBuilder()
    df = pb.get_daily_data(['GOGG', 'AAPL', 'MSFT'], date(2020, 1, 1), date(2020, 2, 1))
    print(df)
    print(pb.days)
    universal = pb.find_universal_portfolio(20)
    exponential_grad = pb.find_exponential_gradient_portfolio()
    print(universal,len(universal))
    print(exponential_grad,len(exponential_grad))