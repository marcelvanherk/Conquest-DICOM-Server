--###############################################
-- RSERVE - Start in its own terminal on debug mode
-- default port 6311
--###############################################
-- R
-- library(Rserve)
-- Rserve(debug=T)
--###############################################
-- TEST STARTS HERE
--###############################################

-- reload package (for testing)
package.loaded.rclient = nil

R = require "rclient"
R.connect()

R("library(survival)")
a = R.evaluate('d <- c(1,2,3,NA); e <- c("red", "white", NA, "blue"); f <- c(NA,TRUE,TRUE,FALSE); mydata <- data.frame(d,e,f)')
printtable(a)
print('----------------')

f = frame:new({{1,5,3},{'a','b','c'},{0,1,2}}, {'time','letters','status'})
--f:readCSV(filename)
R.f = f
R('f$SurvObj <- with(f, Surv(time, status == 2))')
printtable(R.f)
print('----------------')

m = matrix:new({{1,1},{1,-1}})
R.m = m
R('m <- solve(m)')
printtable(R.m)
print('----------------')

f = factor:new({'red', 'green', 'blue', 'red'})
printtable(f)
R._set(R, 'f', f)
printtable(R.f)
print('----------------')

v = {1,2,3,4}
R.v = v
printtable(R.v)
print('----------------')

--R("library(survival)")
--R("data(lung)")
--a = R("lung")

a = R.evaluate('d <- c(1,2,3,NA); e <- c("red", "white", NA, "blue"); f <- c(NA,TRUE,TRUE,FALSE); mydata <- data.frame(d,e,f)')
print(a._class)
printtable(a)
R.a = a
printtable(R.a)
print('----------------')

R.x = a
printtable(R.x)

b = R.evaluate("replicate(5, rnorm(7))")
print(b._class)
printtable(b)
R.b = b
printtable(R.b)
print('----------------')

c = R.evaluate('gender <- c(rep("male",2), rep("female", 3)); gender <- factor(gender)')
print(c._class)
printtable(c)
R.c = c
printtable(R.c)
print('----------------')

-- ask Rserve for 10 random numbers
printtable(R.evaluate("rnorm(10)"))

-- ask Rserve for a vector
printtable(R.evaluate('a <- c(1, 2, 5.3, 6, -2, 4)'))
printtable(R.evaluate('b <- c("one","two","three")'))
printtable(R.evaluate('c <- c(TRUE,TRUE,TRUE,FALSE,TRUE,FALSE)'))

-- ask Rserve for a matrix
printtable(R.evaluate('y <- matrix(1:20, nrow=5, ncol=4)'))
printtable(R.evaluate('y <- matrix(1:20, nrow=5, ncol=4, byrow=TRUE)'))

-- ask Rserve for a data.frame
printtable(R.evaluate('d <- c(1,2,3,4); e <- c("red", "white", "red", NA); f <- c(TRUE,TRUE,TRUE,FALSE); mydata <- data.frame(d,e,f)'))
printtable(R.evaluate('d <- c(1,2,3,4); e <- c("red", "white", "red", NA); f <- c(TRUE,TRUE,TRUE,FALSE); mydata <- data.frame(d,e,f); names(mydata) <- c("ID","Color","Passed"); mydata'))

-- ask Rserve for a factor
c = R.evaluate('gender <- c(rep("male",2), rep("female", 3)); gender <- factor(gender)')
print(c._class)
printtable(c)
printtable(R.evaluate('v <- c(1,3,5,8,2,1,3,5,3,5); x <- factor(v)'))

-- ask Rserve for a data.frame
a = (R.evaluate('n <- c(2, 3, 5); s <- c("aa", "bb", "cc"); b <- c(TRUE, FALSE, TRUE); df <- data.frame(n, s, b)'))
printtable(a)

-- ask Rserve for a linear regression
printtable(R('library(MASS); data(cats); lm(Hwt ~ Bwt, data=cats[1:5,])'))
