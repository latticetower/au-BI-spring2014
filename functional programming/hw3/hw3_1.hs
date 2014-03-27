module Hw3_1 where
import Data.List(nub, permutations, sort)

-- Задание 1: повторите каждый элемент списка заданное число раз
tautologized_list :: Int -> [a] -> [a]
tautologized_list _ [] = []
tautologized_list n l | n < 0 = error "first parameter must be greater than 0"
                      | n == 1 = l
                      | otherwise = concat(map (\x -> take n (repeat x)) l)
                      
-- задание 2: удалите каждый n-ый элемент списка
delete_nth :: Int -> [a] -> [a]
delete_nth _ [] = []
delete_nth n l | n < 0 = error "first parameter must be greater than 0"
               | n == 0 = l
               | otherwise = helper 1 n l
                 where
                  helper :: Int -> Int -> [a] -> [a]
                  helper _ _ [] = []
                  helper i n (x:xs) | i == n = (helper 1 n xs)
                                    | otherwise = x:(helper (i + 1) n xs)
                                    
-- задание 3: выделите подсписок с n-го по k-ый номер [n;k)
sublist :: Int -> Int -> [a] -> [a]
sublist _ _ [] = []
sublist n 0 _ = []
sublist n k l | n < 0 = error "first parameter must be greater than 0"
              | k < 0 = error "second parameter must be greater than 0" 
              | n > k = error "first parameter must be less than second" -- пока этот вариант считаю ошибочным. можно вернуть интервал (k; n]
              | n > 0 = sublist (n - 1) (k - 1) (tail l)
              | otherwise = helper 0 k l
                where 
                  helper :: Int -> Int -> [a] -> [a]
                  helper _ _ [] = []
                  helper _ 0 _ = []
                  helper i k (x:xs) | i == k = []
                                    | otherwise = x:(helper (i + 1) k xs)
-- задание 4: задайте циклическую ротацию списка влево
rotate_left :: Int -> [a] -> [a]
rotate_left _ [] = []
rotate_left k l = (\n -> (sublist (n + k `rem` n) n l) ++ (sublist 0 (n + k `rem` n) l)) (length l)
-- при передаче значений, меньших чем длина списка, они берутся по модулю длины списка
-- при передаче значений, больших 0, возвращается сообщение об ошибке

-- задание 5: Удалите n-ый элемент из списка, вернув его и список
extract_nth :: Int -> [a] -> (a, [a])
extract_nth _ [] = error "List is empty"
extract_nth k l | k < 0 = error "first parameter should be greater or equal than zero"
                | k > (length l) = error "first parameter should be less than list length"
                | otherwise = ((nth_element k l), ((sublist 0 k l) ++ (sublist (k+1) (length l) l)))
                  where 
                    nth_element :: Int -> [a] -> a
                    nth_element _ [] = error "list is empty"
                    nth_element k (x:xs) | k < 0 = error "list index out of bounds"
                                         | k == 0 = x
                                         | otherwise = nth_element (k - 1) xs
-- задание 6: найдите все сочетания из элементов заданного списка по n элементов.
-- можно сделать что-то с методом permutations - для всех циклических сдвигов длины n вызывать этот метод:
nth_combination n l = nub (map sort (map (take n) (permutations l))) 
-- но это не слишком быстро.
nth_combination' _ [] = []
nth_combination' n (x:xs) | n == 0 = []
                          | n == 1 = map (\x->[x]) (x:xs)
                          | otherwise = (map (\xs -> x : xs) (nth_combination' (n - 1) xs)) ++ (nth_combination' n xs) 
--

