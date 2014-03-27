{-# OPTIONS_GHC -XStandaloneDeriving #-}
module Practice3 where
-- определения типов
data Tree a = Leaf a | Branch (Tree a) a (Tree a)
data List a = Nil | Cons a (List a)

-- дальше функции с практики
eq_tree :: Eq a => Tree a -> Tree a -> Bool
eq_tree (Branch l n r) (Branch l2 n2 r2) | n /= n2 = False
                                         | otherwise = (eq_tree l l2) && (eq_tree r r2)
eq_tree (Leaf a) (Leaf b) = a == b
eq_tree _ _ = False

instance Eq a => Eq (Tree a) where 
  (==) = eq_tree

-- реализовать функцию elemTree, определяющую, хранится ли значение в дереве
elemTree :: Eq a => Tree a -> a -> Bool
elemTree (Leaf a) b = a == b
elemTree (Branch l n r) b | n == b = True
                          | otherwise = or [(elemTree l b), (elemTree r b)]
                          
-- сделайте типовый оператор Tree представителем класса типов Functor
instance Functor Tree where
  fmap = func
func :: (a -> b) -> Tree a -> Tree b  
func f (Leaf a) = (Leaf (f a))
func f (Branch l x r) = (Branch (func f l) (f x) (func f r))
--
-- Show


-- напишите версию instance Show для типа List a, чтобы они выводили список в заданном виде  
instance Show a => Show (List a) where
  show = myShowList'
myShowList' :: Show a => List a -> String
myShowList' Nil = "|"
myShowList' (Cons x xs) = "<" 
                          ++ (show x)
                          ++ (myShowList' xs)
                          ++ ">"

-- Напишите две версии instance Show для типа Tree a, так чтобы они выводили дерево в следующем виде
instance Show a => Show (Tree a) where
  show = myShowTree
myShowTree :: Show a => Tree a -> String
myShowTree (Leaf x) = show x
myShowTree (Branch l x r) = "<" 
                          ++ (myShowTree l)
                          ++ "{" ++ (show x) ++ "}"
                          ++ (myShowTree r)
                          ++ ">"
-- вариант 2
data Tree' a = Leaf' a | Branch' (Tree' a) a (Tree' a)

instance Show a => Show (Tree' a) where
  showsPrec _ = myShowsTree'
myShowsTree' :: Show a => Tree' a -> ShowS
myShowsTree' (Leaf' x) =  shows x
myShowsTree' (Branch' l x r) = ('<' :) . ((myShowsTree' l) . (showChar '{' . (shows x . (showChar '}' . ((myShowsTree' r) . showChar '>')))))

--
-- класс типов Read
-- Напишите функцию myReadsTree :: (Read a) => ReadS (Tree a) 

myReadsTree :: (Read a) => ReadS (Tree a)
myReadsTree ('<':s) = [(Branch l x r, u) | (l, '{':s2) <- myReadsTree s, (x, '}':t) <- reads s2, (r, '>':u) <- myReadsTree t ]
myReadsTree (s) = [(Leaf x, t) | (x, t) <- reads s]
