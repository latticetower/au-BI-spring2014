module Hw3_3 where
import Data.List(nub, permutations, sort, partition)
-- переделала задание 4: задайте циклическую ротацию списка влево
-- задание 4: задайте циклическую ротацию списка влево
rotate_left :: Int -> [a] -> [a]
rotate_left _ [] = []
rotate_left k l = helper [] l k 
                    where
                      helper :: [a] -> [a] -> Int -> [a]
                      helper lb rb k | k == 0 = rb ++ lb
                                     | k < 0 = helper lb rb (k + length(lb) + length(rb))
                                     | (null rb) = helper [] lb k -- эта проверка нужна, чтобы при k>=length(l) циклический сдвиг все еще был корректным
                                     | k > 0 = helper (lb ++ [head rb]) (tail rb) (k - 1)
                                     | otherwise = error "something went wrong"
                                     --
-- задачи с деревьями
data Tree a = Empty | Branch (Tree a) a (Tree a) deriving (Show, Eq)
--
-- задача 3: напишите функцию, строящую из списка дерево бинарного поиска
make_bst :: Ord a => [a] -> Tree a
make_bst [] = Empty
make_bst (x:xs) = (\l -> (Branch (make_bst (fst l)) x (make_bst (snd l)))) (partition (\y->y < x) xs)
-- задача 4: напишите функцию, возвращающую список всех сбалансированных по высоте деревьев типа Tree с n узлами

prod' :: [(Int, Tree ())] -> [(Int, Tree ())] -> [(Int, Tree ())] 
prod' xs ys =  nub ([((max (fst x) (fst y) + 1), (Branch (snd x) () (snd y))) | x <- xs, y <- ys, abs((fst x) - (fst y)) <= 1] ++ 
                    [((max (fst x) (fst y) + 1), (Branch (snd y) () (snd x))) | x <- xs, y <- ys, abs((fst x) - (fst y)) <= 1])
-- функция prod' вспомогательная, возвращает список сбалансированных деревьев, принимая на вход списки деревьев меньшей высоты, без дублей и тд
-- возвращает деревья большей высоты, приписывая к всевозможным парам правых и левых поддеревьев вершину-корень
-- проверяла, например, так
-- prod' (bt_helper 1) (bt_helper 2)
bt_helper :: Int -> [(Int, Tree ())] -- возвращает для заданного k список пар - (высота, дерево)
bt_helper 0 = [(0, Empty)]
bt_helper 1 = [(1, (Branch Empty () Empty))]
bt_helper 2 = [(2, (Branch (Branch Empty () Empty) () Empty)), (2, (Branch Empty () (Branch Empty () Empty)))]
bt_helper 3 = [(2, (Branch (Branch Empty () Empty) () (Branch Empty () Empty)))] 
bt_helper n | n < 0 = error "parameter must be greater than zero"
            | otherwise = nub(concat(map (\n1 -> prod' (bt_helper n1) (bt_helper (n - 1 - n1))) (take (n - 2) (iterate succ 1))))
-- список сбалансированных по высоте деревьев с n узлами выводит следующая функция:
bt_list :: Int -> [ Tree () ]
bt_list n = map (snd)(bt_helper n)
