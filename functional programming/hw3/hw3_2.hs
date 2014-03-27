module Hw3_2 where
import Data.List(nub, permutations, sort)
--
-- задачи с деревьями
data Tree a = Empty | Branch (Tree a) a (Tree a) deriving (Show, Eq)
--
-- задача 1: напишите функцию, возвращающую список всех полностью сбалансированных деревьев типа Tree с n узлами
-- вспомогательная функция
build_tree_list :: Int -> [Tree ()]
build_tree_list 0 = [Empty]
build_tree_list 1 = [(Branch (Empty) () (Empty))]
build_tree_list 2 = [(Branch (Branch Empty () Empty) () (Empty)), (Branch (Empty) () (Branch Empty () Empty) )]
build_tree_list k | k < 0 = error "parameter must be greater than zero"
                  | k `mod` 2 == 0 = concat((\k2-> (map (\y->map (\f-> f y) (map (\x y-> (Branch (x) () (y))) (build_tree_list k2)) ) (build_tree_list (k - k2)))
                                          ++ (map (\y->map (\f-> f y) (map (\x y -> (Branch (x) () (y))) (build_tree_list (k - k2)))) (build_tree_list (k2)))
                                      ) (div (k-1) 2))
                  | otherwise = concat(map (\y->map (\f -> f y)(map (\x y -> (Branch x () y)) (build_tree_list (div (k - 1) 2)))) (build_tree_list (div (k - 1) 2)))
--(\k2->[(Branch (build_tree_list k2) () (build_tree_list (k - k2))), (Branch (build_tree_list (k - k2)) () (build_tree_list k2))]) ((k - 1)/2)
                  -- (Branch (build_tree_list ((k - 1)/2)) () (build_tree_list ((k - 1) / 2)))
                  
-- задача 2: Напишите функцию, проверяющую, является ли дерево структурно симметричным относительно корня (значения в узлах не важны)
is_symmetric :: (Tree a) -> Bool
is_symmetric Empty = True
is_symmetric (Branch l _ r) = helper l r
  where
    helper :: (Tree a) -> (Tree b) -> Bool
    helper Empty Empty = True
    helper (Branch l1 _ r1) (Branch l2 _ r2) = and [(helper l1 r2), (helper r1 l2)]
    helper _ _ = False
    
-- задача 3: напишите функцию, строящую из списка дерево бинарного поиска
-- задача 4: напишите функцию, возвращающую список всех сбалансированных по высоте деревьев типа Tree с n узлами
