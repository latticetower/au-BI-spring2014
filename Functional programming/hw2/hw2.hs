module Hw2 where
import Data.Char(digitToInt)
import Data.List(intersect)

-- ������� 1: �������
data Direction = North | West | South | East deriving Show
-- ������� ������
turn_left  :: Direction -> Direction
turn_left North = West
turn_left West = South
turn_left South = East
turn_left East = North
 
-- ������� �������
turn_right :: Direction -> Direction
turn_right North = East
turn_right East = South
turn_right South = West
turn_right West = North

-- ��������
turn_back :: Direction -> Direction
turn_back = \x->turn_left (turn_left x)

-- ������� 2: 
-- ����������� ������ ���� ��������� ������������ �����
digits :: Integer -> [Int]
digits x = (map digitToInt (show (abs x)))

-- ����������, �������� �� �������� ����������� ����� ��� ����� �� 1 �� 9
has_all_digits :: Integer -> Bool
has_all_digits = \x -> length (['1'..'9'] `intersect` (show (abs x))) == 9

-- ����������, �������� �� �������� ����������� ����� ��� ����� �� 1 �� 9 � �������� �� ������ ����.
-- ������� �� �����������: ���� � ����� ���� ��������� ������������� ����, �� ����������� � ������� "123456789" ������ ��� �������.
-- � ������ �������, ���� ���������, ��� ����� ���� 9 �������
has_all_digits' :: Integer -> Bool
has_all_digits' = \x -> length ((show (abs x)) `intersect` ['1'..'9']) == 9 && has_all_digits x

--������� 3: ��� ������ (�������� �����������) [a1, a2, ...] � ����� ����� n. �������� ������ ���������� ���������� ����� n
sliding_lists = \n l -> map (take n) (iterate (\x->if not(null x) then tail x else []) l)
-- �� ����: take 4 $ sliding_lists 6 (iterate (\x-> x+1) 1)
-- �� �������� ������� ������ �������� � ����� �����: take 8 $ sliding_lists 5 [1,2,3,4]) 

sliding_lists' n l | n < 1 = []
                   | null(l) = []
                   | otherwise = (take n l) : (sliding_lists' n (tail l))
                   
-- ������� 4: ���������� ����������� ��� ��� ������������� ���� ��������� ������. �������� ��������� �������:
-- ���������� ����� ��������� ������; ���������� ������������ ������ ������.
data Leaf a = Empty |  Cons Int ((Leaf a), (Leaf a)) deriving (Show, Read, Eq, Ord)  
-- ������ ������ (�������� �����������):
-- (1 `Cons` (2 `Cons` ((3 `Cons` (Empty, Empty)), Empty), Empty))
-- ���������� ����� ��������� ������
sum' :: Leaf a -> Int
sum'  Empty        = 0
sum' (Cons x xs) = x + sum'(fst xs) + sum'(snd xs)

-- ������������ ������ ������
max_len :: Leaf a -> Int
max_len (Cons _ xs) = 1 + max (max_len (fst xs)) (max_len (snd xs))
max_len  Empty        = 0
-- max_len (1 `Cons` (2 `Cons` ((3 `Cons` (Empty, Empty)), Empty), Empty)) - ��������, ���������� 3

doubleElems a = [x*2 | x<-a]