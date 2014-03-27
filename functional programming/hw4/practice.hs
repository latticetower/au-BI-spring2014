{-# OPTIONS_GHC -XStandaloneDeriving #-}
module Practice4 where
import Data.Monoid
-- Моноиды
-- написать 
--newtype Endo a = Endo { appEndo :: a -> a }
--instance Monoid (Endo a ) where
--  mempty  = Endo id
--  Endo f `mappend` Endo g = (Endo f.g)
  
-- Определите тип функции
-- можно ли написать представителя моноида для типа Maybe a сколько разных вариантов можно реализовать
--instance Monoid (Maybe a) where
--  mempty = Nothing
--  Nothing `mappend` Nothing = Nothing
--  (Just a) `mappend` Nothing = (Just a)
--  Nothing `mappend` (Just b) = (Just b)
--  (Just a) `mappend` (Just b) = (Just (a `mappend` b))
  
--instance Monoid (Maybe a) where
--  mempty = Nothing
--  Nothing `mappend` Nothing = Nothing
--  (Just a) `mappend` Nothing = (Just a)
--  Nothing `mappend` (Just b) = (Just b)
--  (Just a) `mappend` (Just b) = first(Just (a `mappend` b))
 -- варианты: first, last в последнем случае
 
 
 -- I (2 балла) Напишите реализацию следующих функций через свёртки foldr или foldr1
length' :: [a] -> Int
length' = foldr (\l c -> c+1) 0 
or' :: [Bool] -> Bool
or' = foldr (\l c-> c || l) False
head' :: [a] -> a
head' = foldr1 (\l c -> l)
last' :: [a] -> a
last' = foldr1 (\l c -> c)
maximum' :: Ord a => [a] -> a
maximum' = foldr1 (\l c -> if c > l then c else l)
map' :: (a -> b) -> [a] -> [b]
map' f = foldr (\a b -> (f a):b) []
filter' :: (a -> Bool) -> [a] -> [a]
filter' p = foldr (\l c -> if (p l) then l:c else c) []


-- I (1 балл) Используя правую свёртку, напишите функцию, конструирующую из списка строк строку, разделённую запятыми
collect' :: [String] -> String
collect' = foldr (\l c -> if c /= "" then l ++ "," ++ c else l) []

-- I (2 балла) Напишите две реализации reverse :: [a] -> [a] — через свёртки foldr и foldl
reverse' :: [a] -> [a]
reverse' = foldr (\l c -> c ++ [l]) []

reverse'' :: [a] -> [a]
reverse'' = foldl (\c l -> [l]++ c) []

-- I (2 балла) Напишите реализацию foldl через foldr.
foldl' f = foldr (flip f)


reverse''' :: [a] -> [a]
reverse''' = foldl' (\l c -> [c]++ l) []

