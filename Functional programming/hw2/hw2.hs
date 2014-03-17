module Hw2 where
import Data.Char(digitToInt)
import Data.List(intersect)

-- Задание 1: локатор
data Direction = North | West | South | East deriving Show
-- поворот налево
turn_left  :: Direction -> Direction
turn_left North = West
turn_left West = South
turn_left South = East
turn_left East = North
 
-- поворот направо
turn_right :: Direction -> Direction
turn_right North = East
turn_right East = South
turn_right South = West
turn_right West = North

-- разворот
turn_back :: Direction -> Direction
turn_back = \x->turn_left (turn_left x)

-- Задание 2: 
-- Сформируйте список цифр заданного натурального числа
digits :: Integer -> [Int]
digits x = (map digitToInt (show (abs x)))

-- Определите, содержит ли заданное натуральное число все цифры от 1 до 9
has_all_digits :: Integer -> Bool
has_all_digits = \x -> length (['1'..'9'] `intersect` (show (abs x))) == 9

-- Определите, содержит ли заданное натуральное число все цифры от 1 до 9 в точности по одному разу.
-- отличие от предыдущего: если в числе есть несколько дублирующихся цифр, то пересечение с строкой "123456789" вернет все повторы.
-- с другой стороны, надо учитывать, что может быть 9 девяток
has_all_digits' :: Integer -> Bool
has_all_digits' = \x -> length ((show (abs x)) `intersect` ['1'..'9']) == 9 && has_all_digits x

--Задание 3: Дан список (возможно бесконечный) [a1, a2, ...] и целое число n. Создайте список скользящих подсписков длины n
sliding_lists = \n l -> map (take n) (iterate (\x->if not(null x) then tail x else []) l)
-- на этом: take 4 $ sliding_lists 6 (iterate (\x-> x+1) 1)
-- на конечных списках теперь работает в такой форме: take 8 $ sliding_lists 5 [1,2,3,4]) 

sliding_lists' n l | n < 1 = []
                   | null(l) = []
                   | otherwise = (take n l) : (sliding_lists' n (tail l))
                   
-- Задание 4: Определите рекурсивный тип для представления узла бинарного дерева. Напишите следующие функции:
-- вычисление суммы элементов дерева; вычисление максимальной высоты дерева.
data Leaf a = Empty |  Cons Int ((Leaf a), (Leaf a)) deriving (Show, Read, Eq, Ord)  
-- пример дерева (выглядит страшновато):
-- (1 `Cons` (2 `Cons` ((3 `Cons` (Empty, Empty)), Empty), Empty))
-- вычисление суммы элементов дерева
sum' :: Leaf a -> Int
sum'  Empty        = 0
sum' (Cons x xs) = x + sum'(fst xs) + sum'(snd xs)

-- максимальная высота дерева
max_len :: Leaf a -> Int
max_len (Cons _ xs) = 1 + max (max_len (fst xs)) (max_len (snd xs))
max_len  Empty        = 0
-- max_len (1 `Cons` (2 `Cons` ((3 `Cons` (Empty, Empty)), Empty), Empty)) - например, возвращает 3

doubleElems a = [x*2 | x<-a]