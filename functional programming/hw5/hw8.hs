module Hw8 where
import Control.Applicative

-- Задание 1 (2 балла): Для двоичного дерева
data Tree a = Nil | Branch (Tree a) a (Tree a) deriving (Show)
-- попробуйте написать
-- instance Applicative Tree where
-- pure = 
-- (<*>) = 
instance Functor Tree where
  fmap g Nil = Nil
  fmap g (Branch lb v rb) = Branch (fmap g lb) (g v) (fmap g rb)
instance Applicative Tree where
  pure x = Branch (pure x) x (pure x)--  
  Nil <*> _ = Nil   
  _ <*> Nil = Nil  
  (Branch lb1 f rb1) <*> (Branch lb2 v rb2) = Branch (lb1 <*> lb2) (f v) (rb1 <*> rb2)

--проверьте, что ваша версия работает, вызвав, например
-- (+) <$> Branch (Branch Nil 1 Nil) 2 Nil
-- <*> Branch (Branch Nil 3 Nil ) 4 (Branch Nil 5 Nil)
-- Сравните результаты работы с instance Applicative для списков.
-- Branch (Branch Nil 4 Nil) 6 Nil -- на тестовом примере возвращает это дерево
-- можно было написать реализацию по аналогии со списками, т.е., например, возвращать дерево, элементами которого являются деревья


----------------------------------
-- Задание 2 (2 балла)
-- Имеется семейство функций zipWith, zipWith3,..
-- Аппликативные функторы могут заменить все это семейство
-- Реализуйте операторы (>*<) и (>$<), позволяющие спраятать упаковку ZipList и распаковку getZipList

(>$<) :: (a -> b) -> [a] -> [b]
f >$< l = getZipList $ f <$> ZipList l
infixl 4 >$< -- вычисления должны проводиться слева направо, поэтому приоритет не должен быть меньше приоритета >*<

(>*<) :: [a -> b] -> [a] -> [b]
f >*< l = getZipList $ ZipList f <*> ZipList l
infixl 4 >*< -- приоритет как у <*> 

----------------------------------
-- Задание 3 (3 балла): введем следующий трехпараметрический типовой оператор, инкапсулирующий композицию однопараметрических конструкторов типов:
-- Каков кайнд этого конструктора типов? Приведите пример замкнутого типа, сконструированного с помощью Compose, и пример терма этого типа
--newtype Compose f g x = Compose{getCompose :: f (g x)}
-- кайнд конструктора типа Compose :: f (g x) -> Compose f g x
--ffmap h = getCompose . fmap h. Compose
-- ffmap :: Functor (Compose f g) => (a -> x) -> f (g a) -> f (g x)
--instance (Functor f, Functor g) => Functor (Compose f g) where
--  fmap s (Compose f) = (Compose (fmap s f))
  ----------------------
-- Задание 4 (3 балла): Проверьте, что законы аппликативных функторов выполняются для Maybe и списков