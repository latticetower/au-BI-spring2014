module Hw10 where
import Control.Monad.State
import  Control.Monad(replicateM)
import Data.IORef
import Control.Monad.Writer
import System.Random
import Data.List(sort, group, intercalate)
import System.IO

--(1 балл) Напишите функцию, вычисляющую факториал с использованием монады State
nextMultiplier :: State (Integer, Integer) Integer
nextMultiplier = do x <- get
                    put ((fst x)*(snd x), (snd x) + 1)
                    return (fst x)

fact n | n >= 0 = fst (execState (replicateM n nextMultiplier )(1,1))
       | otherwise = error "invalid input, provide non-negative integer"

--(1 балл) Напишите функцию, вычисляющую числа Фибоначчи с использованием монады State
nextFib :: State (Integer, Integer) Integer
nextFib = do x <- get
             put ((fst x) + (snd x), (fst x))
             return ((fst x) + (snd x))

fib n | n >= 0 = snd (execState (replicateM n nextFib )(1,0))
      | otherwise = error "invalid input, provide non-negative integer"

--(2 балла) Напишите функцию while::IORef t-> (t->Bool) -> IO () -> IO (),
-- позволяющую описывать "императивные циклы" следующего вида
while :: IORef t -> (t -> Bool) -> IO () -> IO ()
while t f body = do 
              g <- readIORef t
              if (f g) then do
                result <- body
                while t f body 
              else return ()
                    

factorial n = do
  r <- newIORef 1
  i <- newIORef 1
  while i (<= n) ( do
    ival <- readIORef i
    modifyIORef r (* ival)
    modifyIORef i (+ 1)
    )
  readIORef r

--(2 балла) Используя монаду Writer, напишите версию библиотечной функции sum - функцию sumLogged :: Num a => [a] -> Writer String a, 
-- в которой бы рекурсивные типы сопровождались бы записью в лог, так чтобы в результате получалось такое поведение:
sumLogged :: (Num a, Show a) => [a] -> Writer String a
sumLogged [] = writer (0, "0")
sumLogged (x:xs) = do
    let (y, log) = runWriter $ sumLogged xs
    writer (x + y, "(" ++ (show x) ++ "+" ++ log ++ ")")
    
-- (2 балла) Напишите программу, эмулирующую 1000 серий подбрасываний монетки по 1000 раз. Вычислите усредненный по сериям модуль отклонения
-- количества орлов от своего среднего значения в серии (500)
-- вспомогательное: почти то же, что while, но с счетчиком - чтобы не забывать modifyIORef
for :: IORef t -> (t -> Bool) -> (t -> t) -> IO () -> IO ()
for t f inc body = do
  v <- readIORef t
  if (f v) then do
    result <- body
    modifyIORef t inc
    for t f inc body 
  else return ()
  
-- будем считать что орлу соответствует 0, решке - 1
-- вспомогательная функция для получения серии подбрасываний монетки
coin_flips :: Int -> IO [Int]
coin_flips n = replicateM n $ randomRIO (0,1)
--
average = uncurry (/) . foldr (\x (s, c) -> ((fromIntegral x) + s, c + 1.0)) (0.0, 0.0)
--
--coin_flip_series :: Int -> IO Double
coin_flip_series f n = do
  i <- newIORef 1
  d <- newIORef []
  for i (<=n) (+1) ( do
      g <- coin_flips 1000
      let tails = (length . filter (==0)) g
      let tails_d = f (tails - 500)
      modifyIORef d (++ [tails_d])
    )
  h <- readIORef d
  --print "tails deviations from average for all series:"
  --print h
  return h
--запускать coin_flip_series 1000 (ну т.е. в виде функции с параметром - количеством серий)
average_for1000flips = do
  h <- coin_flip_series abs 1000
  return (average h) 
  
-- эта функция выводит среднее по сериям
divergences_for1000flips = do
  h <- coin_flip_series id 1000
  let h_sorted = map (\x->(head x, (length x))) (group (sort h))
  return  h_sorted --метод возвращает массив пар - первый элемент - отклонение, второй - сколько раз встретилось

--(2 балла) запишите в файл в виде гистограммы (ascii-арт)
-- метод пишет в файл гистограмму, параметр - имя файла 
print_ascii_to_file filename = do
  handle <- openFile filename WriteMode
  l <- divergences_for1000flips
  hPutStr handle (intercalate "\n" (map (\x->"" ++ (show (fst x)) ++" " ++ (replicate (snd x) 'x') ) l))
  hClose handle
  print "done"
