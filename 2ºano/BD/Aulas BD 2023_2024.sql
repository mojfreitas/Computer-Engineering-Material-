----------------------------------------------------Aula 1------------------------------------------------------------------------
exec naluno (2021145193);
SELECT * FROM autores;
EXEC SQLCHECK('FAHZSOFACFUDBBX');
Select titulo from Livros order by titulo;
EXEC SQLCHECK('FAJOFSKBMIUXCEZ');
Select distinct genero from livros order by genero;
EXEC SQLCHECK('FAEHGSQCELEEDMQ'); 
Select titulo, genero, preco_tabela  As Preco from Livros where preco_tabela>=30 and preco_tabela<=40 order by preco_tabela;
EXEC SQLCHECK('FAZPHKCDDOXSEUK'); 
----------------------------------------------------------------------------------------------------------------------------------

----------------------------------------------------Aula 2------------------------------------------------------------------------
exec naluno (2021145193);

select titulo, genero, preco_tabela 
from livros 
where (genero = 'Policial' or genero = 'Aventura')
and  preco_tabela > 20 order by titulo;
EXEC SQLCHECK('FBZFTYYCWMJOENW');

select nome, sexo 
from autores
where genero_preferido like 'Romance' and idade > 30 order by nome asc;
EXEC SQLCHECK('FBHYDWADZAUJFAT');

select distinct genero from livros where paginas > 400 or preco_tabela > 25 order by genero;
EXEC SQLCHECK('FBJQNSWEVIFUGWV');

select titulo, preco_tabela As preco from livros where genero = 'Informática' order by preco asc;
EXEC SQLCHECK('FBHOWKTFDMPWHAI');

select titulo, ISBN, quant_em_stock from livros where genero = 'Informática' and quant_em_stock > 10 order by preco_tabela asc, quant_em_stock desc;
EXEC SQLCHECK('FBLPCFWGZYXCITX');

select distinct codigo_autor from livros order by codigo_autor DESC;
EXEC SQLCHECK('FBCLDRYHKXKMJXJ');

select titulo from livros where genero != 'Aventura';
EXEC SQLCHECK('FBELDMQIWUXFKNT');

select titulo, preco_tabela from livros where genero not like 'Aventura';
EXEC SQLCHECK('FBIZEEEJGLOKLVI');

select nome, sexo from autores where genero_preferido = 'Romance' and idade>30 order by nome asc;
EXEC SQLCHECK('FBTNEIDKPDPIMYQ');

select nome, idade from  autores where nome like '%o%' and  nome like '%u%' order by idade;
EXEC SQLCHECK('FBCVUBULKHUONEV');

select titulo, genero from livros where (titulo like 'O%' and titulo like '%st%') or (titulo like 'W%' and titulo like '%NT%') order by preco_tabela;
EXEC SQLCHECK('FBLTKPUMCPJWOJT');

select titulo, genero, preco_tabela from livros where genero = 'Policial' or genero = 'Romance' or genero = 'Economia' order by titulo;
EXEC SQLCHECK('FBWUSOSNTSQGPPG');

select titulo, paginas As "Num de Paginas" from livros where genero = 'Romance' and paginas < 520 order by paginas;
EXEC SQLCHECK('FBHNFMWOMZTHQNJ');

select codigo_livro, titulo, preco_tabela,quant_em_stock As "Quantidade em stock" from livros where genero = 'Policial' order by quant_em_stock;
EXEC SQLCHECK('FBONDSEPBRRKRIO');

select 'O livro "'||titulo||'" tem '|| paginas ||' paginas.' As "Listagem paginas dos Livros" from livros where genero = 'Aventura' order by titulo;
EXEC SQLCHECK('FBTFIETQMZONSSJ');


----------------------------------------------------------------------------------------------------------------------------------

----------------------------------------------------Aula 3------------------------------------------------------------------------
exec naluno (2021145193);

select titulo,genero from livros, autores where preco_tabela < 20 and livros.codigo_autor = autores.codigo_autor and autores.nome = 'J. K. Rowling' order by titulo;
EXEC SQLCHECK('FCINKYPCGVYDGGH');

select titulo, genero, nome, morada from livros, autores where morada like '%Cascais%' and livros.codigo_autor = autores.codigo_autor and livros.genero != 'Romance' order by titulo;
EXEC SQLCHECK('FCEKVCZDJCDXCDF');

select distinct nome, idade, genero_preferido from Livros, Autores where livros.codigo_autor = autores.codigo_autor and livros.genero = autores.genero_preferido order by nome;
EXEC SQLCHECK('FCUXGHVEQFTXWVO');

select titulo, genero, preco_tabela, paginas 
from Livros, Vendas, Clientes 
where livros.codigo_livro = vendas.codigo_livro and vendas.codigo_cliente = clientes.codigo_cliente and clientes.morada like '%Porto%' and Livros.PAGINAS>300 order by titulo;
EXEC SQLCHECK('FCOAPRNFTZUAMIP');

select nome, genero_preferido, titulo, genero As "Genero do Livro"
from livros, autores 
where livros.codigo_autor = autores.codigo_autor and livros.genero != autores.genero_preferido and preco_tabela < 40 order by nome, titulo;
EXEC SQLCHECK('FCTVMKWGHTEVJOA');

select distinct TITULO,PRECO_TABELA,UNIDADES_VENDIDAS, round(preco_tabela * unidades_vendidas * 0.20, 2) as "Rendeu"
from livros
WHERE genero = 'Fantástico'
order by 4 desc;
EXEC SQLCHECK('FCGBQWUHPLOLVGV');

select distinct autores.nome, autores.idade, autores.morada
from autores, clientes
where autores.n_contribuinte = clientes.n_contribuinte
order by 1;
EXEC SQLCHECK('FCHIEOUIJMGUZSG');


select titulo, genero, preco_tabela, 
floor(unidades_vendidas * preco_tabela * 0.2) as "Rendimento"
from livros
where (unidades_vendidas * preco_tabela * 0.2) >= 400000 
order by 4 desc ,3;
EXEC SQLCHECK('FCTTXUUJYDKYYCO');

select titulo, preco_tabela, paginas,round((preco_tabela/(paginas/100)),2) as "Custo página",ceil(preco_tabela/(paginas/100)) as "Custo pág.(sup)",floor(preco_tabela/(paginas/100)) as "Custo pág.(inf)"
from livros
where paginas BETWEEN 100 and 300
order by 1;
EXEC SQLCHECK('FCQVTXHKQBHFJBZ');

select lower (titulo) as "Titulo (em minusculas)", upper (titulo) as "Titulo (em maiusculas)", initcap (titulo) as "Titulo (1a letra maiuscula)"
from livros
where genero like 'Informática'
order by titulo;
EXEC SQLCHECK('FCFXAVLLOEBMIEN');

SELECT TITULO AS "Titulo", PRECO_TABELA AS "Preco", QUANT_EM_STOCK AS "Quantidade em stock"
FROM LIVROS
WHERE GENERO LIKE 'Romance' 
ORDER BY 3 DESC;
EXEC SQLCHECK('FCROYAZMYRXAHEL');

SELECT NOME AS "Nome completo", SUBSTR(NOME, 0 , INSTR(NOME, ' ') -1) AS "Primeiro Nome", 
SUBSTR(NOME, INSTR(NOME, ' ', -1) +1) AS "Ultimo Nome"
FROM AUTORES 
WHERE GENERO_PREFERIDO LIKE 'Informática'
ORDER BY IDADE, 1;
EXEC SQLCHECK('FCVMAFTNZJENMQU');

SELECT LOWER(LIVROS.TITULO) AS "Titulo (em minusculas)", UPPER(LIVROS.GENERO) AS "genero (maisculas)", 
INITCAP(AUTORES.NOME) AS "Nome Autor (1a letra)" 
FROM LIVROS, AUTORES, EDITORAS
WHERE LIVROS.CODIGO_AUTOR = AUTORES.CODIGO_AUTOR AND 
LIVROS.GENERO = AUTORES.GENERO_PREFERIDO AND 
EDITORAS.NOME LIKE 'FCA - EDITORA' AND 
LIVROS.CODIGO_EDITORA = EDITORAS.CODIGO_EDITORA 
ORDER BY 1 DESC;
EXEC SQLCHECK('FCKUJPVODLJWHHD');

SELECT TITULO, GENERO
FROM LIVROS
WHERE LENGTH(TITULO) > 35 AND TITULO LIKE '%f%'
ORDER BY 1;
EXEC SQLCHECK('FCYYGHXPVYZLYEN');

SELECT TITULO, PRECO_TABELA AS "PRECO", ROUND((PRECO_TABELA * 0.08) + PRECO_TABELA, 1) AS "PRECO_COM_AUMENTO"
FROM LIVROS
WHERE GENERO LIKE 'Fantástico' AND PRECO_TABELA > 20
ORDER BY 3 DESC;
EXEC SQLCHECK('FCGQIBVQANPMAAK');
----------------------------------------------------------------------------------------------------------------------------------


----------------------------------------------------Aula 4------------------------------------------------------------------------
exec naluno(2021145193);

Select titulo,genero,preco_tabela from livros where TO_CHAR(data_edicao,'yyyy') = '2020' order by data_edicao desc;
EXEC SQLCHECK('FDTWGOFCTGHDGFG');

SELECT TO_CHAR(SYSDATE,'DDth, Month, YYYY') "DATA Atual", TO_CHAR(SYSDATE,'HH24:MI:SS AM') "Hora Min Sec Atual" FROM DUAL;
EXEC SQLCHECK('FDVOLRHDBSMRHUO');

SELECT titulo As "Titulo", PRECO_TABELA AS "Preco", TO_CHAR(DATA_EDICAO,'DD-MM-YYYY') "DATA_EDICAO"
FROM LIVROS 
WHERE SYSDATE - DATA_EDICAO < 180;
EXEC SQLCHECK('FDMNLCZEJJLUIAV');

SELECT DISTINCT L.TITULO, TO_CHAR(DATA_EDICAO, 'DD-MM-YYYY') AS "DATA"
FROM LIVROS L
JOIN VENDAS V ON L.CODIGO_LIVRO = V.CODIGO_LIVRO
JOIN CLIENTES C ON V.CODIGO_CLIENTE = C.CODIGO_CLIENTE
WHERE C.MORADA LIKE '%Lisboa%'
AND V.DATA_VENDA BETWEEN TO_DATE('2014-01-15', 'YYYY-MM-DD') AND TO_DATE('2014-02-18', 'YYYY-MM-DD')
ORDER BY 2, 1;
EXEC SQLCHECK('FDKWDJZFBNMKJXK');

SELECT L.TITULO, TO_CHAR(L.DATA_EDICAO, 'YYYY-MM') AS "ANO_MES"
FROM LIVROS L, VENDAS V 
WHERE L.GENERO LIKE 'Informática' 
AND TO_CHAR(L.DATA_EDICAO, 'YYYY-MM') = TO_CHAR(V.DATA_VENDA, 'YYYY-MM') 
AND L.CODIGO_LIVRO = V.CODIGO_LIVRO
ORDER BY 2;
EXEC SQLCHECK('FDOVPNBGJYYXKBW');

SELECT DISTINCT A.NOME
FROM AUTORES A, LIVROS L
WHERE L.PAGINAS < 630 AND 
L.CODIGO_AUTOR = A.CODIGO_AUTOR AND
L.GENERO LIKE 'Informática' AND
L.DATA_EDICAO BETWEEN TO_DATE('2013-12-21', 'YYYY-MM-DD') AND TO_DATE('2014-03-21', 'YYYY-MM-DD')
ORDER BY 1;
EXEC SQLCHECK('FDHHZHYHUIKALHN');

SELECT L.GENERO, L.TITULO AS "Titulo", TO_CHAR(DATA_EDICAO, 'DD-MM-YYYY') AS "DATA_EDICAO", 
TRUNC(MONTHS_BETWEEN(SYSDATE , L.DATA_EDICAO) / 12) AS "Num. de anos"
FROM LIVROS L
WHERE INITCAP(GENERO) LIKE 'Romance' OR INITCAP(GENERO) LIKE 'Aventura'  
ORDER BY 1, 2;
EXEC SQLCHECK('FDLGBFHIIJNSMYO');

SELECT C.NOME
FROM CLIENTES C, VENDAS V
WHERE C.CODIGO_CLIENTE = V.CODIGO_CLIENTE AND  
V.DATA_VENDA = NEXT_DAY(V.DATA_VENDA - 1, 7) AND 
TO_CHAR(V.DATA_VENDA, 'HH24') > '20';
EXEC SQLCHECK('FDSINCQJUJVPNCI');

SELECT NOME, IDADE
FROM AUTORES
MINUS
SELECT A.NOME, A.IDADE
FROM AUTORES A, LIVROS L
WHERE L.CODIGO_AUTOR = A.CODIGO_AUTOR
ORDER BY 2;
EXEC SQLCHECK('FDKXFDFKEVZVOTU');

select nome from autores a
minus
select nome from autores a, livros l
where a.codigo_autor = l.codigo_autor and (sysdate-l.data_edicao)/365 > 1
order by nome desc;
EXEC SQLCHECK ('FDBJQVPLQMMGPLE');





----------------------------------------------------------------------------------------------------------------------------------

----------------------------------------------------Aula 5------------------------------------------------------------------------
exec naluno (2021145193);

select count(*) As "Num Livros de Informática"
from livros
where genero = 'Informática' and preco_tabela >= 20 and preco_tabela <= 40;
EXEC SQLCHECK('FEKPMBHCGFHKHRG');

select count(*) As "Num Livros Editados", Sum(unidades_vendidas) As "Total Unidades vendidas", avg(preco_tabela) As "Preço de Tabela Médio"
from livros
where genero in ('Romance','Aventura');
EXEC SQLCHECK('FEQZNHADADLXIRE');

select genero, count(*) As "Num. Livros Editados"
from livros
group by genero
order by 2 desc, genero asc;
EXEC SQLCHECK('FETUCAREYKNOJEP');

select TITULO, 
Min(preco_unitario) As PRECO_MAIS_BAIXO,
Max(preco_unitario) As PRECO_MAIS_ALTO, 
Round(AVG(preco_unitario),2) As PRECO_MEDIO
from livros, vendas
where genero = 'Informática' and livros.codigo_livro = vendas.codigo_livro
group by titulo
order by 3 desc, titulo asc;
EXEC SQLCHECK('FEMKYWMFLFBFKTH');

SELECT GENERO, COUNT(*) AS N_LIVROS_EDITADOS, MAX(preco_tabela) - MIN(preco_tabela) AS DIFERENCA_MAX_MIN
FROM LIVROS
GROUP BY GENERO
ORDER BY 3 DESC;
EXEC SQLCHECK('FEAUZTGGYJWBLSU');

SELECT TITULO, PRECO_TABELA , SUM(QUANTIDADE) AS "NUM_VENDIDOS", 
SUM(QUANTIDADE * PRECO_TABELA) AS "RECEITA_ESPERADA", SUM(QUANTIDADE * PRECO_UNITARIO) AS "RECEITA_EFECTIVA"
FROM LIVROS, VENDAS
WHERE LIVROS.CODIGO_LIVRO = VENDAS.CODIGO_LIVRO
AND TO_CHAR(VENDAS.DATA_VENDA,'YYYY') = '2022'
GROUP BY TITULO,PRECO_TABELA
ORDER BY NUM_VENDIDOS;
EXEC SQLCHECK('FELBAFEHXKEYMHB');

SELECT GENERO, ceil(AVG(preco_tabela)) AS "PRECO Médio"
FROM livros
GROUP BY genero
HAVING COUNT(*) > 3
ORDER BY 2 DESC, GENERO;
EXEC SQLCHECK('FEMDMOHIGBIPNYO');

SELECT AUTORES.NOME, MIN(LIVROS.PRECO_TABELA) AS "Preco do Mais Barato", MAX(LIVROS.PRECO_TABELA) AS "Preco do Mais Caro"
FROM AUTORES, LIVROS 
WHERE LIVROS.CODIGO_AUTOR = AUTORES.CODIGO_AUTOR
GROUP BY AUTORES.NOME HAVING MIN(LIVROS.PRECO_TABELA) BETWEEN 10 AND 25 AND MIN(LIVROS.PRECO_TABELA) < MAX(LIVROS.PRECO_TABELA)
ORDER BY "Preco do Mais Barato", AUTORES.NOME;
EXEC SQLCHECK('FEFWWIEJQMYQOLI');

SELECT CLIENTES.NOME, SUM(VENDAS.QUANTIDADE) AS "N.Livros Comprados", ROUND(AVG(VENDAS.PRECO_UNITARIO), 1) AS "Preco Medio",
COUNT(DISTINCT LIVROS.CODIGO_AUTOR) AS "N.Autor Diferent"
FROM CLIENTES, LIVROS, VENDAS 
WHERE CLIENTES.CODIGO_CLIENTE = VENDAS.CODIGO_CLIENTE
AND LIVROS.CODIGO_LIVRO = VENDAS.CODIGO_LIVRO
AND UPPER(CLIENTES.MORADA) NOT LIKE '%LISBOA%'
GROUP BY CLIENTES.CODIGO_CLIENTE, CLIENTES.NOME
HAVING COUNT(DISTINCT LIVROS.CODIGO_EDITORA) <= 1
ORDER BY CLIENTES.NOME;
EXEC SQLCHECK('FEXIBSQKCAWKPBN');

SELECT AUTORES.NOME, REPLACE(COUNT(CODIGO_LIVRO), 0, 'Nenhum') AS "Num de Livros escritos"
FROM AUTORES , LIVROS 
WHERE AUTORES.CODIGO_AUTOR = LIVROS.CODIGO_AUTOR(+)
AND AUTORES.IDADE > 50
GROUP BY AUTORES.NOME
HAVING COUNT(DISTINCT LIVROS.TITULO) <= 3
ORDER BY 1;
EXEC SQLCHECK('FEKXNTJLDGJOQTJ');

----------------------------------------------------------------------------------------------------------------------------------

----------------------------------------------------Aula 6------------------------------------------------------------------------
exec naluno (2021145193);
SELECT TITULO, PRECO_TABELA AS "Preco_Mais_Baixo"
FROM LIVROS
WHERE GENERO = 'Fantástico' AND PRECO_TABELA = (SELECT MIN(PRECO_TABELA) FROM LIVROS WHERE GENERO = 'Fantástico');
EXEC SQLCHECK('FFKMXQQCCWIZINA');

SELECT TITULO, PAGINAS "N_Paginas", PRECO_TABELA "Preco_mais_Baixo"
FROM LIVROS
WHERE GENERO = 'Fantástico' AND PRECO_TABELA = (Select MIN(PRECO_TABELA) FROM LIVROS WHERE GENERO = 'Fantástico');
EXEC SQLCHECK('FFDJTIRDJJNSJGQ');
 
SELECT  PRECO_TABELA "Preco_Mais_Baixo",TITULO
FROM LIVROS L1
WHERE GENERO = 'Fantástico' AND NOT EXISTS (
    SELECT 1
    FROM LIVROS L2
    WHERE L2.GENERO = 'Fantástico' AND L2.PRECO_TABELA < L1.PRECO_TABELA
    );
EXEC SQLCHECK('FFZMRWEEXQNJKIQ');

SELECT L.TITULO "Livro Mais Barato"
FROM LIVROS L
JOIN (
    SELECT GENERO, MIN(PRECO_TABELA) AS PRECO_MIN
    FROM LIVROS
    WHERE GENERO = 'Fantástico'
    GROUP BY GENERO
) SUBQ
ON L.GENERO = 'Fantástico' AND L.PRECO_TABELA = SUBQ.PRECO_MIN;
EXEC SQLCHECK('FFOIHIAFORVLLSJ');

SELECT DISTINCT A.NOME
FROM AUTORES A
WHERE A.CODIGO_AUTOR IN (
    SELECT L.CODIGO_AUTOR
    FROM LIVROS L
    WHERE L.PAGINAS > (SELECT AVG(PAGINAS) FROM LIVROS)
)
order by nome;
EXEC SQLCHECK('FFBKSPCGAZHRMTE');

SELECT DISTINCT A.NOME, A.IDADE, A.NACIONALIDADE
FROM LIVROS L
INNER JOIN AUTORES A ON L.CODIGO_AUTOR = A.CODIGO_AUTOR
WHERE A.CODIGO_AUTOR IN (
  SELECT AUTORES.CODIGO_AUTOR
  FROM AUTORES, LIVROS
  WHERE LIVROS.CODIGO_AUTOR = AUTORES.CODIGO_AUTOR
  GROUP BY AUTORES.CODIGO_AUTOR
  HAVING COUNT(LIVROS.CODIGO_LIVRO) > (
    SELECT AVG(QUANTLIVROS)
    FROM (
      SELECT AUTORES.CODIGO_AUTOR, COUNT(LIVROS.CODIGO_LIVRO) AS QUANTLIVROS
      FROM AUTORES, LIVROS
      WHERE LIVROS.CODIGO_AUTOR = AUTORES.CODIGO_AUTOR
      GROUP BY AUTORES.CODIGO_AUTOR
    ) 
  )
);
EXEC SQLCHECK('FFIIHBGHVJSENIL');

select distinct 
livros.codigo_autor,
livros.titulo, livros.preco_tabela
as "PRECO", aa.pt as
"PRECO_MINIMO",
livros.preco_tabela - aa.pt as "DIFERENCA_PRECO"
from livros, (select L.codigo_autor, min(L.preco_tabela) pt
from livros L
group by
L.codigo_autor) aa
where genero like 'Fantástico'
and livros.CODIGO_AUTOR =
aa.codigo_autor
order by 1,2;
EXEC SQLCHECK('FFHAWDXINTAMOTO');

select genero, titulo, unidades_vendidas
from livros
where (genero,unidades_vendidas)
in (select 
genero, min(UNIDADES_VENDIDAS)
unidades_vendidas
from livros
Group by Genero)
order by unidades_vendidas DESC;
EXEC SQLCHECK('FFEFGOFJFOVYPMP');

select L.Titulo, to_char(round(100
*L.Unidades_Vendidas / B.Soma,
1), '990.99') as PERCENTAGEM
from Livros L, Editoras E, (select sum(L.Unidades_Vendidas) as SOMA
from Livros L, Editoras E
where L.Codigo_editora = E.CODIGO_EDITORA
and upper(E.Nome) like '%GRADIVA%') B
where L.Codigo_Editora = E.CODIGO_EDITORA
and upper(E.Nome) like '%GRADIVA%'
order by 2 Desc, 1;
EXEC SQLCHECK('FFUSZQRKODGHQZN');

select L.Titulo,L.GENERO
from Livros L, Clientes C, vendas V
where L.Codigo_livro =
V.CODIGO_LIVRO
and V.CODIGO_CLIENTE =
C.CODIGO_CLIENTE
and upper(C.Morada) like '%LISBOA%'
and V.QUANTIDADE = (Select
max(quantidade)
from Clientes C, Vendas v
where V.codigo_cliente =
C.CODIGO_CLIENTE
and upper(C.Morada) like
'%LISBOA%');
EXEC SQLCHECK('FFEATZOLGSNMRRV');

----------------------------------------------------------------------------------------------------------------------------------

----------------------------------------------------Aula 7------------------------------------------------------------------------
exec naluno (2021145193);
SELECT TITULO, SOMA_QUANTIDADE
FROM (
    SELECT L.TITULO, SUM(V.QUANTIDADE) AS SOMA_QUANTIDADE,
           ROW_NUMBER() OVER (ORDER BY SUM(V.QUANTIDADE) DESC, L.TITULO ASC) AS RN
    FROM LIVROS L
    JOIN VENDAS V ON L.CODIGO_LIVRO = V.CODIGO_LIVRO
    GROUP BY L.TITULO
) 
WHERE RN = 1;
EXEC SQLCHECK('FGQWRMHCENGCJFH');

WITH LivrosVendidos AS (
    SELECT 
        L.GENERO,
        L.TITULO,
        L.PAGINAS,
        COALESCE(SUM(V.QUANTIDADE), 0) AS TOTAL_VENDIDO
    FROM LIVROS L
    LEFT JOIN VENDAS V ON L.CODIGO_LIVRO = V.CODIGO_LIVRO
    WHERE L.PAGINAS > 600
    GROUP BY L.GENERO, L.TITULO, L.PAGINAS
)

SELECT GENERO, TITULO, PAGINAS
FROM (
    SELECT 
        GENERO,
        TITULO,
        PAGINAS,
        TOTAL_VENDIDO,
        ROW_NUMBER() OVER (PARTITION BY GENERO ORDER BY TOTAL_VENDIDO ASC) AS RN
    FROM LivrosVendidos
) 
WHERE RN = 1
ORDER BY GENERO, TITULO;
EXEC SQLCHECK('FGQQTBADOXSZKMA');

select l.titulo, a.nome, l.unidades_vendidas
from livros l, autores a,(select codigo_autor, max(unidades_vendidas) m
from livros
group by codigo_autor)aa
where l.codigo_autor = aa.codigo_autor
and a.codigo_autor = l.codigo_autor
and l.unidades_vendidas = aa.m
order by a.nome, l.titulo;
EXEC SQLCHECK('FGJZUXGEZPNYLKV');

SELECT 
    e.NOME AS NOME,
    l.TITULO,
    a.NOME AS NOME_AUTOR,
    l.UNIDADES_VENDIDAS
FROM 
    LIVROS l
JOIN 
    EDITORAS e ON e.CODIGO_EDITORA = l.CODIGO_EDITORA
JOIN 
    AUTORES a ON a.CODIGO_AUTOR = l.CODIGO_AUTOR
JOIN 
    (SELECT 
        EDITORAS.NOME AS EN, 
        MAX(UNIDADES_VENDIDAS) AS M
     FROM 
        LIVROS
     JOIN 
        EDITORAS ON LIVROS.CODIGO_EDITORA = EDITORAS.CODIGO_EDITORA
     GROUP BY 
        EDITORAS.NOME) aa ON l.UNIDADES_VENDIDAS = aa.M AND e.NOME = aa.EN
ORDER BY 
    e.NOME, 
    l.TITULO;
EXEC SQLCHECK('FGKMUHRFQTBIMZF');

SELECT C.CODIGO_CLIENTE AS "Código do Cliente", C.NOME AS "Nome", SUM(V.QUANTIDADE) AS NUM_LIVROS_COMPRADOS
FROM VENDAS V
JOIN CLIENTES C ON V.CODIGO_CLIENTE = C.CODIGO_CLIENTE
WHERE V.DATA_VENDA BETWEEN TO_DATE('2020-10-01', 'YYYY-MM-DD') AND TO_DATE('2020-12-31', 'YYYY-MM-DD')
GROUP BY C.CODIGO_CLIENTE, C.NOME
HAVING SUM(V.QUANTIDADE) = (
    SELECT MAX(SUM_QUANTIDADE)
    FROM (
        SELECT SUM(QUANTIDADE) AS SUM_QUANTIDADE
        FROM VENDAS
        WHERE DATA_VENDA BETWEEN TO_DATE('2020-10-01', 'YYYY-MM-DD') AND TO_DATE('2020-12-31', 'YYYY-MM-DD')
        GROUP BY CODIGO_CLIENTE
    )
)
ORDER BY C.NOME;
EXEC SQLCHECK('FGOEDODGBGAONUP');

WITH MaxVendas AS (
    SELECT 
        CODIGO_EDITORA, 
        MAX(UNIDADES_VENDIDAS) AS Max_Unid_Vendidas
    FROM 
        LIVROS
    GROUP BY 
        CODIGO_EDITORA
)

SELECT 
    a.NOME AS Nome,
    e.NOME AS "Nome Editora",
    l.UNIDADES_VENDIDAS AS UNID_VENDIDAS
FROM 
    LIVROS l
JOIN 
    AUTORES a ON l.CODIGO_AUTOR = a.CODIGO_AUTOR
JOIN 
    EDITORAS e ON l.CODIGO_EDITORA = e.CODIGO_EDITORA
JOIN 
    MaxVendas mv ON l.CODIGO_EDITORA = mv.CODIGO_EDITORA
WHERE 
    l.UNIDADES_VENDIDAS >= 0.7 * mv.Max_Unid_Vendidas
ORDER BY 
    a.NOME, 
    e.NOME;
       EXEC SQLCHECK('FGYRVQIHKSBOOLN');
       
       WITH VendaMaxima2022 AS (
    SELECT 
        CODIGO_LIVRO, 
        MAX(QUANTIDADE) AS Max_Quantidade
    FROM 
        VENDAS
    WHERE 
        EXTRACT(YEAR FROM DATA_VENDA) = 2022
    GROUP BY 
        CODIGO_LIVRO
    ORDER BY 
        Max_Quantidade DESC
)

, VendaTop AS (
    SELECT CODIGO_LIVRO, Max_Quantidade
    FROM (
        SELECT CODIGO_LIVRO, Max_Quantidade
        FROM VendaMaxima2022
        WHERE ROWNUM = 1
    )
)

SELECT 
    l.TITULO,
    l.GENERO,
    a.NOME
FROM 
    LIVROS l
JOIN 
    AUTORES a ON l.CODIGO_AUTOR = a.CODIGO_AUTOR
JOIN 
    VendaTop vt ON l.CODIGO_LIVRO = vt.CODIGO_LIVRO
ORDER BY 
    a.NOME, 
    l.TITULO;
EXEC SQLCHECK('FGSZIMRISBUYPFF');


select clientes.nome, livros.titulo, sum(vendas.quantidade) as "QUANT_COMPRADA"
from livros, clientes, vendas
where livros.codigo_livro = vendas.codigo_livro
and vendas.codigo_cliente = clientes.codigo_cliente
and vendas.codigo_cliente = (select codigo_cliente
                            from vendas
                            where preco_unitario < 30
                            group by codigo_cliente
                            having sum(vendas.quantidade) = (select max(sum(vendas.quantidade))
                                                        from vendas
                                                        where vendas.preco_unitario < 30
                                                        group by vendas.codigo_cliente))
group by livros.titulo, clientes.nome
order by 1,2;
    EXEC SQLCHECK('FGKIKZOJTUGEQTK');


select a.codigo_autor, a.nome, quantidade, L.TITULO
from autores a, livros l, vendas v
where a.codigo_autor = l.codigo_autor
and l.codigo_livro = v.codigo_livro
and l.codigo_autor in (
    select l.codigo_autor
    from livros l, vendas v,(
    select min(count(v.codigo_venda)) m
    from livros l, vendas v
    where l.codigo_livro = v.codigo_livro
    group by l.codigo_autor
    ) tabl
    where l.codigo_livro = v.codigo_livro
    group by l.codigo_autor, tabl.m
    having count(v.codigo_venda)=tabl.m
    )
    order by 2;
    EXEC SQLCHECK('FGHHHQDKNXLPRFD');
    
----------------------------------------------------------------------------------------------------------------------------------

----------------------------------------------------Aula 8------------------------------------------------------------------------

exec naluno (2021145193);
SELECT Autor.NOME AS Nome,
       Livro.TITULO,
       Livro.LIVROS_PORTO_EDITORA,
       TotalLivrosAutor.TOTAL_DE_LIVROS
FROM (
    SELECT A.CODIGO_AUTOR, A.NOME
    FROM Autores A
) Autor
JOIN (
    SELECT L.CODIGO_AUTOR, L.TITULO, COUNT(*) AS LIVROS_PORTO_EDITORA
    FROM Livros L
    JOIN Editoras E ON L.CODIGO_EDITORA = E.CODIGO_EDITORA
    WHERE E.NOME = 'PORTO EDITORA'
    AND L.PRECO_TABELA = (
        SELECT MIN(PRECO_TABELA)
        FROM Livros
        WHERE CODIGO_EDITORA = E.CODIGO_EDITORA
    )
    GROUP BY L.CODIGO_AUTOR, L.TITULO
) Livro ON Autor.CODIGO_AUTOR = Livro.CODIGO_AUTOR
JOIN (
    SELECT CODIGO_AUTOR, COUNT(*) AS TOTAL_DE_LIVROS
    FROM Livros
    GROUP BY CODIGO_AUTOR
) TotalLivrosAutor ON Autor.CODIGO_AUTOR = TotalLivrosAutor.CODIGO_AUTOR
WHERE ROWNUM = 1
ORDER BY TotalLivrosAutor.TOTAL_DE_LIVROS DESC;

EXEC SQLCHECK('FHPIETTCGQOCKNR');


SELECT 
    a.nome AS NOME, 
    CASE 
        WHEN a.genero_preferido IS NOT NULL THEN 
            (SELECT COUNT(*) FROM livros l WHERE l.codigo_autor = a.codigo_autor AND l.genero = a.genero_preferido)
        ELSE 
            0 
    END AS "Genero Preferido",
    (SELECT COUNT(*) FROM livros l WHERE l.codigo_autor = a.codigo_autor) AS "Total de Livros"
FROM 
    autores a
ORDER BY NOME;

EXEC SQLCHECK('FHFMLMIDVHAALUN');


WITH EditoraMaisVendida AS (
  SELECT LIVROS.CODIGO_EDITORA
  FROM LIVROS
  GROUP BY LIVROS.CODIGO_EDITORA
  HAVING SUM(LIVROS.UNIDADES_VENDIDAS) = (
    SELECT MAX(VENDAS_TOTAL)
    FROM (
      SELECT CODIGO_EDITORA, SUM(UNIDADES_VENDIDAS) AS VENDAS_TOTAL
      FROM LIVROS
      GROUP BY CODIGO_EDITORA
    )
  )
)

SELECT 'O autor ' || AUTORES.NOME || ' escreveu ' || COUNT(LIVROS.CODIGO_AUTOR) || ' de ' || 
       (SELECT COUNT(*) FROM LIVROS WHERE CODIGO_AUTOR = AUTORES.CODIGO_AUTOR) || 
       ' livros para a editora FCA - EDITORA.' AS RESULTADO
FROM AUTORES
INNER JOIN LIVROS ON AUTORES.CODIGO_AUTOR = LIVROS.CODIGO_AUTOR
WHERE LIVROS.CODIGO_EDITORA IN (SELECT CODIGO_EDITORA FROM EditoraMaisVendida)
GROUP BY AUTORES.NOME, AUTORES.CODIGO_AUTOR
ORDER BY AUTORES.NOME ASC; 

EXEC SQLCHECK('FHQVHSKECGWIMOH');


SELECT 
    'top ' || sub.ranking AS top,
    sub.titulo,
    sub.total_vendido AS soma
FROM (
    SELECT 
        l.titulo,
        SUM(v.quantidade) as total_vendido,
        RANK() OVER (ORDER BY SUM(v.quantidade) DESC) as ranking
    FROM 
        livros l
    JOIN 
        vendas v 
        ON l.codigo_livro = v.codigo_livro
    WHERE 
        EXTRACT(YEAR FROM v.data_venda) = EXTRACT(YEAR FROM SYSDATE) - 1
    GROUP BY 
        l.titulo
) sub
WHERE sub.ranking <= 3;

EXEC SQLCHECK('FHFMHGHFGBZWNSR');


WITH MaxPurchaseDates AS (
  SELECT CLIENTES.CODIGO_CLIENTE, MAX(VENDAS.DATA_VENDA) AS MaxDate
  FROM CLIENTES
  INNER JOIN VENDAS ON CLIENTES.CODIGO_CLIENTE = VENDAS.CODIGO_CLIENTE
  GROUP BY CLIENTES.CODIGO_CLIENTE
)

SELECT C.NOME, L.TITULO, TO_CHAR(MPD.MaxDate, 'DD-MM-YYYY') AS DATA_DA_VENDA
FROM MaxPurchaseDates MPD
JOIN CLIENTES C ON MPD.CODIGO_CLIENTE = C.CODIGO_CLIENTE
JOIN VENDAS V ON C.CODIGO_CLIENTE = V.CODIGO_CLIENTE AND MPD.MaxDate = V.DATA_VENDA
JOIN LIVROS L ON V.CODIGO_LIVRO = L.CODIGO_LIVRO
ORDER BY C.NOME ASC;

EXEC SQLCHECK('FHQSWMFGQHNBOVG');


WITH 
VendasMensais AS (
    SELECT
        TO_CHAR(Data_Venda, 'YYYY/MM') AS AnoMes,
        SUM(Quantidade) AS QuantidadeTotal
    FROM
        Vendas
    GROUP BY
        TO_CHAR(Data_Venda, 'YYYY/MM')
), V2 as (
    SELECT
        TO_CHAR(add_months (Data_Venda,1), 'YYYY/MM') AS AnoMes,
        SUM(Quantidade) AS QuantidadeTotal
    FROM
        Vendas
    GROUP BY
        TO_CHAR(add_months (Data_Venda,1), 'YYYY/MM')
)
SELECT
    VM.AnoMes AS Mes,
    VM.QuantidadeTotal AS Vendas_Do_Mes,
    NVL(VA.QuantidadeTotal, 0) AS Vendas_Do_Mes_Anterior,
    CASE
        WHEN VA.QuantidadeTotal IS NOT NULL THEN
            ROUND((VM.QuantidadeTotal - VA.QuantidadeTotal) / VA.QuantidadeTotal * 100, 2)
        ELSE
            NULL
    END AS Variacao_Percent
FROM
    VendasMensais VM
LEFT JOIN
    V2 VA ON VM.AnoMes = VA.AnoMes
WHERE
    VA.QuantidadeTotal IS NOT NULL
    AND (VM.QuantidadeTotal - VA.QuantidadeTotal) / VA.QuantidadeTotal * 100 < -50
ORDER BY
    VM.AnoMes;
    
EXEC SQLCHECK('FHNFTPUHLPYZPFU');


----------------------------------------------------------------------------------------------------------------------------------

----------------------------------------------------Aula 10-----------------------------------------------------------------------

exec naluno (2021145193);
------ex3--------------
Create table autores2 (
    CODIGO_AUTOR     NUMBER(4,0)   CONSTRAINT pk_autores2 primary key,
    NOME             VARCHAR2(30) constraint NN_nome not null,
    N_CONTRIBUINTE     NUMBER(9,0)   not null, --UNIQUE,-- UNIQUE NOT NULL e uma chave candidata(CK) constraint NN_n_Cont 
    MORADA             VARCHAR2(50),
    IDADE             NUMBER(4,0)   CONSTRAINT CK_IDADE CHECK(idade between 0 and 199),
    SEXO             CHAR(1),
    NACIONALIDADE     VARCHAR2(20),
    GENERO_PREFERIDO VARCHAR2(20),
    CONSTRAINT ck_aut2_nome CHECK(nome=upper(nome)),
    CONSTRAINT UK_NIF  UNIQUE(N_CONTRIBUINTE)
);
EXEC SQLCHECK('FJURGEYCRPWBCQL');
 --drop table autores2;
 
 
------ EX4
CREATE TABLE AVALIACOES(
   CODIGO_LIVRO     NUMBER(4),
   CODIGO_CLIENTE   NUMBER(4),
   NOTA             NUMBER(1),
   CONSTRAINT PK_AVALIACOES PRIMARY KEY(CODIGO_LIVRO, CODIGO_CLIENTE),
   CONSTRAINT FK_AVALIACOES_LIVRO FOREIGN KEY(CODIGO_LIVRO) REFERENCES  LIVROS(CODIGO_LIVRO),
   CONSTRAINT FK_AVALIACOES_CLIENTE FOREIGN KEY(CODIGO_CLIENTE) REFERENCES CLIENTES(CODIGO_CLIENTE),
   CONSTRAINT CK_AVALIACOES_NOTA CHECK(NOTA BETWEEN 1 AND 5));
EXEC SQLCHECK('FJPJGYEDBOGCGUG');
   
----- EX 5
CREATE TABLE LIVROS_BACKUP AS SELECT
    codigo_livro,
    codigo_editora,
    codigo_autor,
    titulo,
    isbn,
    UPPER(genero) AS GENERO,
    preco_tabela,
    paginas,
    quant_em_stock,
    unidades_vendidas,
    data_edicao
FROM
    livros;
EXEC SQLCHECK('FJCSVXDEHOMCUDH');

------- EX 6
ALTER TABLE LIVROS_BACKUP
    ADD CONSTRAINT PK_LIVRO_BACKUP PRIMARY KEY(CODIGO_LIVRO);
ALTER TABLE LIVROS_BACKUP
    ADD CONSTRAINT FK_LIVROBK_AUTORES FOREIGN KEY(CODIGO_AUTOR) REFERENCES AUTORES(CODIGO_AUTORES);
ALTER TABLE LIVROS_BACKUP
    ADD CONSTRAINT FK_LIVROBK_EDITORA FOREIGN KEY(CODIGO_EDITORA) REFERENCES EDITORAS(CODIGO_EDITORAS);
   EXEC SQLCHECK('FJTVXTKFXANHWRJ');
   
------ EX 7
--INSERT INTO PESSOAS VALUES (11,'ANA', 10) EVITAR ESSA ABORDAGEM
--INSERT INTO PESSOAS(ID, NOME, IDADE) VALUES (11,'ANA', 10) SE NAO SOBER O CAMPO COLOCAR NULL
--INSERT INTO PESSOAS(ID, NOME, IDADE) VALUES (11,'ANA', NULL)
--INSERT INTO PESSOAS(ID, NOME) VALUES (11,'ANA') COLOCA NULL OU O DEFAULT DA TABELA
-- SELECT CODIGO_AUTOR NOME
--FROM AUTORES

INSERT INTO LIVROS_BACKUP VALUES(
    51,
    (SELECT CODIGO_EDITORA
    FROM EDITORAS
    WHERE NOME LIKE UPPER('FCA%')),
    (SELECT CODIGO_AUTOR
    FROM AUTORES
    WHERE UPPER(NOME) = 'SÉRGIO SOUSA'),
    'Informatica para todos',
    132434,
    'Informática',
    24,
    430,
    null,
    0,
    SYSDATE);
EXEC SQLCHECK('FJEEBXSGVMHBUGB');


------ EX 8
INSERT INTO AUTORES2 VALUES(
    45,
    'JOSÉ DE MAGALHÃES',
    77665544,
    NULL,NULL,NULL,NULL,NULL);
EXEC SQLCHECK('FJSHIAGHEFTFMOU');


----------EX 9
DELETE LIVROS_BACKUP 
WHERE CODIGO_LIVRO = 51;
EXEC SQLCHECK('FJIDOBHIRHQKMGJ');

------ EX 10
UPDATE LIVROS_BACKUP
SET PRECO_TABELA = (PRECO_TABELA*1.1)
WHERE GENERO = 'AVENTURA';
EXEC SQLCHECK('FJENOMGJBYRFHME');

------ EX 11
DELETE LIVROS_BACKUP
WHERE GENERO = 'INFORMÁTICA' AND
PRECO_TABELA < (SELECT AVG(PRECO_TABELA)
                FROM LIVROS_BACKUP
                WHERE GENERO = 'INFORMÁTICA');
EXEC SQLCHECK('FJJMILYKNGTVTXU');

---- EX 12
CREATE TABLE AUTORES_BACKUP AS SELECT * FROM AUTORES;
EXEC SQLCHECK('FJPRIYCLMEJDIQJ');

------- EX 13
ALTER TABLE AUTORES_BACKUP
ADD NLIVROS NUMBER(3, 0) NOT NULL CHECK(NLIVROS BETWEEN 0 AND 151); 
EXEC SQLCHECK('FJPOALWMHGZEKYT');

------- EX 14
update autores_backup
set nlivros = (select sum(codigo_livro)
from livros
where livros.codigo_autor = autores_backup.codigo_autor
group by livros.codigo_autor);
EXEC SQLCHECK('FJBDINYNMKKXOAI');
----15
update autores_backup
set genero_preferido = (select genero_preferido
                        from autores
                        where autores.codigo_autor = autores_backup.codigo_autor);
EXEC SQLCHECK('FJUPXPGOXYJXIMH');



----EX16-----
CREATE TABLE EDITORAS_BACKUP AS SELECT * FROM EDITORAS;
EXEC SQLCHECK('FJKTIDMPBAXIQHZ');

----ex17-----
drop TABLE EDITORAS_BACKUP;
EXEC SQLCHECK('FJJJWGBQFLHDFMC');

---- EX18
ALTER TABLE LIVROS_BACKUP
ADD EDICAO NUMBER(7);
EXEC SQLCHECK('FJHIWSKRECYOZQH');
---- EX19
update livros_backup
set unidades_vendidas = (select sum(codigo_venda)
from vendas
where vendas.codigo_livro = livros_backup.codigo_livro
group by vendas.codigo_livro);
EXEC SQLCHECK('FJHBQOGSCBQNXYY');

---- EX20
update vendas_backup
set total_venda = (select sum(vendas.quantidade * vendas_backup.preco_unitario)
from vendas
where vendas.codigo_livro = vendas_backup.codigo_livro
group by vendas.codigo_livro);
EXEC SQLCHECK('FJNOVBYTDNUBGAE');
-----EX21------
CREATE TABLE VENDAS_BACKUP AS SELECT * FROM VENDAS;
EXEC SQLCHECK('FJKHRKTUSCQOKBD');

-----EX22------
delete from vendas_backup
where to_char(data_venda,'MM') = '01';
EXEC SQLCHECK('FJMLDZHVGGVXVBN');
-----EX23------
delete from vendas_backup
where preco_unitario = (select max(preco_tabela)
                        from livros, autores
                        where livros.codigo_autor = autores.codigo_autor
                        and autores.nome like '%Sérgio%Sousa%'
and vendas_backup.codigo_livro = livros.codigo_livro
)
10.txt
5 KB;
EXEC SQLCHECK('FJSPUXLWQFIIVAE');



----------------------------------------------------------------------------------------------------------------------------------

----------------------------------------------------Aula 11-----------------------------------------------------------------------

exec naluno (2021145193);

-----EX3------

CREATE SEQUENCE exp_sequencia; /*Cria a sequência exp_sequência que começa a 1 e incrementa de 1 em 1*/
SELECT exp_sequencia.CURRVAL FROM dual; -- Erro pois a sequência não foi inicializada
SELECT exp_sequencia.NEXTVAL FROM dual; -- Inicia a sequência e devolve o valor atual 1
SELECT exp_sequencia.NEXTVAL FROM dual; -- Incrementa a sequência e devolve o valor atual 2
SELECT exp_sequencia.NEXTVAL from dual; -- Incrementa a sequência e devolve o valor atual 3
SELECT exp_sequencia.CURRVAL FROM dual; -- Devolve o valor atual da sequência
DROP SEQUENCE exp_sequencia; -- Apaga a sequência
EXEC SQLCHECK('FKGFNXWCTITWNME');

-----EX4------

drop table EDITORAS_BACKUP
CREATE TABLE EDITORAS_BACKUP AS (SELECT * FROM EDITORAS);
EXEC SQLCHECK('FKNJNXHDQJLUSPV');

-----EX5------

Alter table EDITORAS_BACKUP ADD(
CONSTRAINT PK_ID_EDITORA_BK PRIMARY KEY (CODIGO_EDITORA),
CONSTRAINT UK_N_CONTRIB_BK UNIQUE (N_CONTRIBUINTE));
EXEC SQLCHECK('FKYEXNXEUROHELV');

-----EX6------

CREATE SEQUENCE   seq_editback
START WITH 10 
INCREMENT BY 1
NOCYCLE
NOMAXVALUE;
EXEC SQLCHECK('FKYDNPXFPBWPIRC');


-----EX7------

Alter Table EDITORAS_BACKUP MODIFY(MORADA VARCHAR(60));
INSERT INTO EDITORAS_BACKUP (ID_EDITORA, NOME, TELEFONE, MORADA, N_CONTRIBUINTE, FAX)
VALUES (seq_editback.nextval, 'D.Quixote', 901111111, 'Rua Cidade de Córdova, n.2 2610-038 Alfragide', 707252252, 707252253);
EXEC SQLCHECK('FKIVZGPGIOZKVZL');

-----EX8------

INSERT INTO EDITORAS_BACKUP VALUES(seq_editback.nextval, 'Almedina',  901212121, 'Rua Fernandes Tomás, n.º 76 a 80, 3000-167 Coimbra, Portugal', 239851903, 239851904);
EXEC SQLCHECK('FKAQCEVHKASRDLO');

-----EX9------

select seq_editback.currval from dual;
select seq_editback.nextval from dual;
select seq_editback.currval from dual;
EXEC SQLCHECK('FKNNXQTIRKBLACD');

-----EX10------

drop SEQUENCE seq_editback;
EXEC SQLCHECK('FKEPPMCJBROFZHT');

-----EX11------

CREATE VIEW LIVROS_INFORMATICA AS
SELECT * FROM LIVROS
WHERE upper(genero) = 'INFORMÁTICA';
EXEC SQLCHECK('FKNAOLLKSLABNKG');

-----EX12------

--A tabela livros_informatica terá dois novos registos, com os dados dos livros ‘Uma noite de Verão’ e ‘O céu é azul’. Estes livros pertencem ao género de informática e romance, respectivamente.
--A tabela livros terá um novo registo, com os dados do livro ‘Longe de tudo’. Este livro pertence ao género de informática e tem o código da editora 2 e o código do autor 2. Estes códigos devem corresponder a registos existentes nas tabelas editoras e autores, respectivamente, para garantir a integridade referencial

-----EX13------

DELETE FROM LIVROS
WHERE CODIGO_LIVRO IN (50, 51, 52);
EXEC SQLCHECK('FKAQXPRMLGWNPXE');
-----EX14------

Drop View livros_informatica;
EXEC SQLCHECK('FKJZDYONFMBBLTS');

-----EX15------

DROP TABLE LIVROS_BACKUP;
Create Table livros_backup AS (select * from livros);
EXEC SQLCHECK('FKZBSHOOZEWQOTY');

-----EX16------

CREATE VIEW AUTOR_LIVRO AS
SELECT AU.NOME, L.TITULO
FROM AUTORES AU, LIVROS_BACKUP L
WHERE AU.CODIGO_AUTOR = L.CODIGO_AUTOR;
EXEC SQLCHECK('FKUAXXVPLXQVHWW');

-----EX17------

SELECT * FROM AUTOR_LIVRO;
EXEC SQLCHECK('FKUZJQEQHXZGJJH');

-----EX18------

DELETE LIVROS_BACKUP;
EXEC SQLCHECK('FKDGLKJRGRCADEV');

-----EX19------

SELECT * FROM AUTOR_LIVRO; 
--Não é possível vizualizar nada, porque a vista não guarda informação e apagamos os registos na tabela original
EXEC SQLCHECK('FKBMGXFSRZOUQYF');

-----EX20------

DROP LIVROS_BACKUP;
EXEC SQLCHECK('FKASZYETZHOLOEE');

-----EX21------

SELECT * FROM AUTOR_LIVRO; 
--Deu ERRO, pois a View está a tentar aceder a uma tabela que não existe
EXEC SQLCHECK('FKGRFMEUNATDSFN');

-----EX22------

CREATE VIEW LIVROS_VENDIDOS AS
SELECT L.TITULO, AU.NOME, SUM(V.QUANTIDADE) AS QUANT_VEND
FROM LIVROS L, VENDAS V, AUTORES AU
WHERE L.CODIGO_LIVRO = V.CODIGO_LIVRO
AND L.CODIGO_AUTOR = AU.CODIGO_AUTOR
GROUP BY V.CODIGO_LIVRO, L.TITULO, AU.NOME
ORDER BY QUANT_VEND DESC;
EXEC SQLCHECK('FKNWSKQVNMPQGVI');

-----EX23------

SELECT * FROM USER_TABLES;
EXEC SQLCHECK('FKTDWYWWPTIMSOZ');

-----EX24------

SELECT * FROM USER_CONSTRAINTS;
EXEC SQLCHECK('FKFEPNHXETKIOZG');

---------------