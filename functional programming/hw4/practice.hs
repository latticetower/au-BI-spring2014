module Practice4 where
import Data.Monoid
import Data.List(insert)
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
reverse'' = foldl (\c l -> [l] ++ c) []

-- I (2 балла) Напишите реализацию foldl через foldr.
-- первое отличие, которое бросается в глаза - это порядок аргументов в входной функции. 
-- можно попробовать применять к ней функцию flip, но
-- foldl' f = foldr (flip f)
-- - так все равно не работает, поскольку foldl и foldr применяют эту функцию к элементам списка в разном порядке. 
-- значит, надо изменить порядок элементов еще и в списке! то есть использовать функцию reverse через foldr, определенную выше.
foldl' = \f x l -> foldr (flip f) x (reverse' l) -- в принципе reverse' можно расписать через foldr
-- для проверки, reverse''' с использованием этой функции:
reverse''' :: [a] -> [a]
reverse''' = foldl' (\l c -> [c]++ l) []

-- (3 балла) Напишите реализацию функции foldTree - свертки для двоичного дерева
data Tree a = Nil | Branch (Tree a) a (Tree a) deriving (Eq, Show)
-- сколько разных версий такой функции вы можете предложить?
-- Используя foldTree, напишите функцию flattenTree:: Tree a->[a]
-- Сколько разных версий такой функции вы можете написать?

-- Решение
-- сходу можно написать 3!=6 версий, характеризующихся разным порядком обхода дерева
-- g :: b -> a -> b -- b- аккумулятор, a - элемент дерева
-- вариант 1
foldTree' :: (b -> a -> b) -> b -> Tree a -> b
foldTree' _ x Nil = x  
foldTree' g x (Branch lb v rb) = foldTree' g (g (foldTree' g x rb) v) lb
-- вариант 2
foldTree'' :: (b -> a -> b) -> b -> Tree a -> b
foldTree'' _ x Nil = x  
foldTree'' g x (Branch lb v rb) = foldTree'' g (g (foldTree'' g x lb) v) rb

-- вариант 3
foldTree''' :: (b -> a -> b) -> b -> Tree a -> b
foldTree''' _ x Nil = x  
foldTree''' g x (Branch lb v rb) = g (foldTree''' g (foldTree''' g x rb) lb) v

-- вариант 4
foldTree'4 :: (b -> a -> b) -> b -> Tree a -> b
foldTree'4 _ x Nil = x  
foldTree'4 g x (Branch lb v rb) = g (foldTree'4 g (foldTree'4 g x lb) rb) v

-- вариант 5
foldTree'5 :: (b -> a -> b) -> b -> Tree a -> b
foldTree'5 _ x Nil = x  
foldTree'5 g x (Branch lb v rb) = foldTree'5 g (foldTree'5 g (g x v) rb) lb

-- вариант 6
foldTree'6 :: (b -> a -> b) -> b -> Tree a -> b
foldTree'6 _ x Nil = x  
foldTree'6 g x (Branch lb v rb) = foldTree'6 g (foldTree'6 g (g x v) lb) rb
-- еще 6 вариантов можно написать, взяв в качестве функции f = flip g (то есть функцию с другим порядком аргументов)
-- еще можно в качестве функции-обработчика взять функцию, принимающую аккумулятор, результат обработки правого поддерева, 
-- результат обработки левого поддерева и значение в узле. Но я пока не придумала, как ее лучше написать 
--(и по какому принципу в этом случае увеличивать аккумулятор).

flattenTree :: Tree a -> [a]
flattenTree = foldTree' (\acc v -> acc ++ [v]) []

flattenTree' :: Ord a => Tree a -> [a]
flattenTree' t = (foldTree'5 (flip insert ) [] t)
-- ну и то же самое можно написать с разными вариантами foldTree...