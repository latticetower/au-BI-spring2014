module Hw9 where
import Control.Monad

--(1 балл) Повторите каждый элемент списка заданное число раз, используя монаду списка и do-нотацию
repeater n l = l >>= \x-> take n (repeat x)

repeater' _ [] = []
repeater' n l = do 
    x <- l
    take n(repeat x)

--(1 балл) Разложите число на два сомножителя всевозможными способами, используя монаду списка и do-нотацию
cofactors n = [1..(div n 2)] >>= (\x-> 
              [div n x] >>= (\y->
              return ((x*y == n) && (x <= y)) >>= (\r ->
              case r of True -> return (x, y)
                        _ -> fail "something went wrong" )))

cofactors' n = do 
    x <- [1..(div n 2)] 
    y <- [div n x] 
    True <- return ((x*y == n) && (x <= y))
    return (x, y)
    
--(2 балла) Вычислите модули разностей между соседними элементами списка, используя монаду списка и do-Нотацию
diffs l = [0..((length l) - 2)] >>= (\i ->
    [l !! i] >>= (\x ->
    [l !! (i + 1)] >>= (\y->
    return (abs(x - y)) )))

diffs' l = do
    i <- [0..(length l) - 2]
    x <- [l !! i]
    y <- [l !! (i + 1)]
    return (abs (x-y))
    
diffs'' l = (iterate succ 0) >>= (\i-> [l !! i] >>= (\x -> [l !! (x + 1)] >>= (\y-> return (abs(x-y)))))
--
diffs''' [] = []
diffs''' (x:[]) = [x]
diffs''' (x:y:xs) = do
    l <- (y:xs)
    x <-l
    return (abs(x-y))
--(2 балла) Покажите, что каждая монада - это аппликативный функтор
--(4 балла) Переведите законы класса Monad на язык join и fmap, используя представление >=> через join и fmap