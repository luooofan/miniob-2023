create table create_view_t1(id int, age int, name char(10));
create table create_view_t2(id int, age int, name char(10));
create view create_view_v1 as select * from create_view_t1;
create view create_view_v2(id, age) as select id, age from create_view_t1;
create view create_view_v3 as select * from create_view_t1 where id > 199;
create view create_view_v4 as select t1.id as id, t1.age as age, t2.name as name from create_view_t1 t1, create_view_t2 t2 where t1.id=t2.id;
create view create_view_v5 as select id, age, id+age as data from create_view_t1;
create view create_view_v7 as select count(t1.id) as num, sum(t1.age)+sum(t2.age) as data from create_view_t1 t1, create_view_t1 t2 where t1.id=t2.id;

insert into create_view_v1 values(78, 78, 'KYXAU');
insert into create_view_v1 values(179, 179, 'YU5DQ1');
insert into create_view_v1 values(77, 77, '7QX1');
insert into create_view_v1 values(120, 120, 'T3DNVGXJ');
insert into create_view_v1 values(70, 70, '5QEOU03D');
insert into create_view_v2 values(67, 67);
insert into create_view_v4 values(99, 100, 'TABLE2');

insert into create_view_t2 values(78, 78, 'KYXAU');
insert into create_view_t2 values(179, 179, 'YU5DQ1');
insert into create_view_t2 values(77, 77, '7QX1');
insert into create_view_t2 values(120, 120, 'T3DNVGXJ');
insert into create_view_t2 values(70, 70, '5QEOU03D');

insert into create_view_v5 (id, age) values(1, 2);
insert into create_view_v2 values(2, 2);

select * from create_view_t1;
select * from create_view_t2;

select count(*) from create_view_v2;
select count(id) from create_view_v2;
select sum(age) from create_view_v2;
select count(name) from create_view_v2;
select count(*) from create_view_v3;
select count(*) from create_view_v4;
select * from create_view_v4;
select sum(data) from create_view_v5;
select count(*) from create_view_v7;
select sum(num) from create_view_v7;
