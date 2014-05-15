module Hw11 where
import Control.Monad
import Control.Monad.Error

import Data.IORef

--(2 балла) Какие из законов класса типов MonadPlus выполняются для списка? типа Maybe? Приведите доказательство или опровергающий пример
-- Решение
-- 1. Список:
-- left zero: выполняется
-- ([] >>= \x->[x]) == [] 
-- left distribution выполняется:
--("aa" ++ "bcd" >>= \x->[x]) == (("aa" >>= \x->[x]) ++ ("bcd" >>= \x->[x])) --left distribution ok
-- left catch flow: не выполняется!
-- ((return "abcd") ++ (return "cdef")) == (return "abcd") -- не выполняется
-- 2. Maybe:
-- left zero: выполняется
-- (Nothing >>= id) == Nothing 
-- left distribution:
mplus :: Maybe a -> Maybe a -> Maybe a
mplus Nothing b = b
mplus a _ = a 
--
-- left catch flow:
--(2 балла) Разработайте функции
--(вспомогательный кусок)
data ParseError = Err {location :: Int, reason :: String}
instance Error ParseError where 
  noMsg = Err 0 "Parse Error"
  strMsg s = Err 0 s

type ParseMonad = Either ParseError
----
--функции:
--parseHex :: String -> ParseMonad Integer
--используем isHexDigit, digitToInt
--parseHex "" = 0
-- parseHex s = get
--printError ::ParseError -> ParseMonad String

--test s = str where
--  (Right str) = do
--    n <- parseHex s
--    return $ show n
--  `catchError` printError

--(2 балла) Используя пользовательский тип ошибки и трансформер ErrorT, модифицируйте этот код так, 
-- чтобы он выдавал пользователю сообщение о причине, по которой пароль отвергнут
