module Practice2 where
import Data.List(mapAccumL)

-- задание 1 - найти количество четных элементов в списке
evens :: [Int] -> Int
evens [] = 0
evens (x:xs) = (if (mod x 2) == 0 then 1 else 0) + evens xs

-- задание 2 - сформируйте новый список, содержащий только нечетные элементы

odd_list :: [Int] -> [Int]
odd_list [] = []
odd_list (x:xs) = if (mod x 2) == 0 then odd_list xs else x: odd_list xs

-- задание 3: сформируйте новый список, в котором переставлены четные и нечетные элементы

swaped_list :: [a] -> [a]
swaped_list [] = []
swaped_list [x] = [x]
swaped_list (x:y:xs) = y : x : swaped_list xs

-- задание 4: даны два списка, сформировать список, 
-- элементы которого равны сумме элементов исходных списков

summed_list :: [Int] -> [Int] -> [Int]
summed_list (x) [] = x
summed_list [] (x) = x
summed_list (x:xs) (y:ys) = (x + y) : summed_list xs ys

-- задание 5: поменяйте порядок элементов списка на противоположный

reversed_list :: [a] -> [a]
reversed_list [] = []
reversed_list (x:xs) = rev [x] xs
  where 
    rev :: [a] -> [a] -> [a]
    rev l [] = l
    rev l (x:xs) = rev (x:l) xs

-- задание 6: сформируйте список, содержащий подсписок длины n исходного списка,
-- начиная с k-того элемента исходного.
sublist :: Int -> Int -> [a] -> [a]
sublist _ _ [] = []
sublist k n (x:xs) | k < 0 = error "invalid parameter 1"
                   | k > 0 = sublist (k - 1) n xs
                   | k == 0 = prefix n (x:xs)
                      where 
                        prefix :: Int -> [a] -> [a]
                        prefix _ [] = []
                        prefix n (x:xs) | n > 0 = x : (prefix (n - 1) xs)
                                        | otherwise = []

----------------------------------------------------------
-- вторая часть
-- задание 1: дан список целых чисел. Увеличить элементы в два раза
doubled l = map (\x->2*x) l

----------------------------------------------------------
-- задание 2: дан список целых чисел. увеличить все его элементы с четными значениями в два раза
evens_doubled l = map (\x-> if (mod x 2) == 0 then 2*x else x) l

----------------------------------------------------------
-- задание 3: дан список целых чисел. Обнулить все его элементы с нечетными значениями
evens_zeroed l = map (\x -> if (mod x 2) == 0 then 0 else x) l

----------------------------------------------------------
-- задание 4: дан список целых чисел. Построить список пар: элемент, его порядковый номер
-- вариант 1:
list_with_indexes l = zip (take (length l) (iterate succ 0)) l
-- вариант 1б (с учетом особенностей функции zip, предыдущий вариант можно написать короче):
list_with_indexes' l = zip (iterate succ 0) l
-- вариант 2:
list_with_indexes'' l = map (\i->(i, l !! i)) (take (length l) (iterate succ 0))
-- вариант 3 (близкий по смыслу к вариантам 1 и 2):
list_with_indexes''' l = take (length l) $ iterate (\x -> (\i->(i, l!!i)) $! succ (fst x) ) (0, l !! 0)
-- вариант 4:
list_with_indexes'''' l = snd (mapAccumL (\i x -> (succ i, (i, x)) ) 0 l)


-----------------------------------------------------------
-- задание 5: Дан список целых чисел. Обнулить все его элементы с нечетными порядковыми номерами
list_without_odds l = map (\i->if i `mod` 2 == 1 then 0 else l !! i) (take (length l) (iterate succ 0))
list_without_odds' l = snd (mapAccumL (\i x -> (succ i, if i `mod` 2 == 1 then 0 else x) ) 0 l)

----------------------------------------------------------
-- задание 6: Дан список целых чисел. Удалить из него элементы, большие заданного числа k.
list_filtered_by k l = filter (\x -> x <= k) l
----------------------------------------------------------
-- задание 7: Даны три списка целых чисел. Составить список сумм соответствующих элементов этих списков
sum3 x y z = zipWith3 (\x y z->x + y + z) x y z
