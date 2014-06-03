module Hw11 where
import Control.Monad
import Control.Monad.Error
import Data.Char(isHexDigit, digitToInt, isNumber, isPunctuation)
import Control.Monad.Trans.Maybe

--(2 балла) Какие из законов класса типов MonadPlus выполняются для списка? типа Maybe? Приведите доказательство или опровергающий пример
-- Решение
-- 1. Список:
-- left zero - выполняется
-- ([] >>= \x->[x]) == [] 
-- left distribution - выполняется:
--("aa" ++ "bcd" >>= \x->[x]) == (("aa" >>= \x->[x]) ++ ("bcd" >>= \x->[x])) --left distribution ok
-- left catch flow - не выполняется!
-- ((return "abcd") ++ (return "cdef")) == (return "abcd") -- не выполняется

-- 2. Maybe (в прошлый раз не доделала):
-- left zero: выполняется
-- (Nothing >>= id) == Nothing 
-- left distribution:
--mplus :: Maybe a -> Maybe a -> Maybe a
--mplus Nothing b = b
--mplus a _ = a 
--
-- left catch flow:


--(2 балла) Разработайте функции
--(вспомогательный кусок)
data ParseError = Err {location :: Int, reason :: String} deriving Show
instance Error ParseError where 
  noMsg = Err 0 "Parse Error"
  strMsg s = Err 0 s

type ParseMonad = Either ParseError
----
--функции:
parseHex :: String -> ParseMonad Integer
parseHex s = helper s 1 0
  where
    helper :: String -> Int -> Integer -> ParseMonad Integer
    helper "" pos res = Right res
    helper (x:xs) pos res = if (isHexDigit x) then (helper xs (pos + 1) (res*16 + (fromIntegral $ digitToInt x)))
              else (throwError (Err pos ([x] ++ ": invalid digit")))

printError ::ParseError -> ParseMonad String
printError  err = return ("At pos " ++ (show $ location err) ++ ": " ++ (reason err))

test s = str where
  (Right str) = do
      n <- parseHex s
      return $ show n
    `catchError` printError