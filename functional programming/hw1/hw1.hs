module Hw1 where
import Data.Char(digitToInt)
 
-- Задание 1: реализуйте функцию, описывающую плотность равномерного распределения на промежутке от 3 до 4
density x = helper 3 4 x
  where helper a b x | x < a = 0
                     | x > b = 0
                     | otherwise = 1.0/(b - a)
  
-- Задание 2: реализуйте функцию, находящую НОД с помощью алгоритма Евклида
gcd' a b = if b /= 0 then gcd' b (mod a b) else a
          
-- Задание 3: реализуйте функции, находящие сумму и количество цифр заданного числа
-- f_digits - вспомогательная функция
-- g - функция двух аргументов. принимает acc и очередную цифру
f_digits g x = helper g 0 (abs x) 10
  where helper g acc x radix | x > 0 = helper g (g acc (mod x radix)) (div x radix) radix
                             | otherwise = acc
-- функция для нахождения суммы цифр:
sum' = f_digits (\acc digit -> acc + digit)
-- функция для нахождения количества цифр:
count' = f_digits (\acc digit -> acc + 1)

-- еще один вариант решения, с использованием библиотечных функций


 
sum'' x = sum (map digitToInt (show (abs x)))
count'' x = length (map digitToInt (show (abs x)))
                           
-- Задание 4: реализуйте функцию, находящую элементы рекуррентной последовательности
seq1 k = helper 1 2 3 k
  where helper a0 a1  a2 k | k > 2 = helper a1  a2 (a2 + a1 - 2 * a0) (k - 1)
                           | k == 2 = a2
                           | k == 1 = a1 
                           | k == 0 = a0

-- Задание 5: Реализуйте функцию, находящую значение определённого интеграла 
-- от заданной функции на заданном интервале по методу трапеций.
-- входные параметры: функция f(x), границы интервала a и b
-- вспомогательная функция: дополнительно принимает число интервалов, на которых считаются трапеции
integral' f a b n = ((b - a) / n) * (helper ((f a + f b) / 2.0) f (a + (b - a) / n) ((b - a) / n) (n - 1))
  where helper acc f x step i | i > 0 = helper (acc + f x) f (x + step) step (i - 1)
                              | otherwise = acc
integral f a b = integral' f a b 10

-- Задание 6: Реализуйте функцию, находящую значение квадратного корня методом Ньютона.
-- вспомогательная функция: считает методом секущих корень уравнения f(x)=0 с заданной точностью
newton f = helper f 0 1 0.000001
  where helper f x_prev x e | (abs (x - x_prev)) < e = x
                            | otherwise = helper f x (x - (f x) / ((f x) - (f x_prev)) * (x - x_prev)) e 

square_root y0 = if y0 >= 0 then newton (\x->x^2 - y0) else error "square root is undefined for negative values"