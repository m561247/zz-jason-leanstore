source("../common.r", local = TRUE)
setwd("../tpcc")
# TPC-C C: 100 warehouses, 120 threads
# 4 combinations of enabled/disabled space/contention management
                                        # C_short_threads.csv C_new.csv have the latest data

dev.set(0)

#df=read.csv('./intel/C_intel_long.csv')
#df=read.csv('./C_rome_1000.csv')
#df=read.csv('./C_mutex_overnight.csv')
#df=read.csv('./C_rome_short.csv')
#df=read.csv('./C_mutex_overnight.csv')
#df=read.csv('./tmp_stats.csv')
c1=read.csv('./C_t120_splitfalse_mergefalse_stats.csv')
c2=read.csv('./C_t120_splittrue_mergefalse_stats.csv')
c3=read.csv('./C_t120_splitfalse_mergetrue_stats.csv')
c4=read.csv('./C_t120_splittrue_mergetrue_stats.csv')
df=sqldf("
select * from c1 UNION
select * from c2 UNION
select * from c3 UNION
select * from c4
")
df=sqldf("select * from df where t >0 ")
d= sqldf("
select *, 1 as variant from df where c_su_merge=0 and c_cm_split=0
UNION select *, 2 as variant from df where c_su_merge=0 and c_cm_split=1
UNION select *, 3 as variant from df where c_su_merge=1 and c_cm_split=0
UNION select *, 4 as variant from df where c_su_merge=1 and c_cm_split=1
")
acc=sqldf("select t,c_worker_threads,tx,space_usage_gib,c_su_merge,c_cm_split,variant,sum(tx/1.0/1e6) OVER (PARTITION BY variant, c_worker_threads ORDER BY t ROWS BETWEEN UNBOUNDED PRECEDING AND CURRENT ROW) as txacc from d where c_worker_threads group by t,tx, c_su_merge,c_cm_split, variant,space_usage_gib,c_worker_threads order by t asc")
head(acc)
outofmemory = sqldf("select a.txacc, o.* from (select variant,c_su_merge,c_cm_split,tx, min(t) as t, c_worker_threads, space_usage_gib from d where space_usage_gib > c_dram_gib group by variant, c_worker_threads order by t asc) o, acc a where a.t=o.t and a.c_cm_split=o.c_cm_split and a.c_su_merge=o.c_su_merge and a.c_worker_threads = o.c_worker_threads")

tx <- ggplot(acc, aes(txacc, tx, color=factor(variant), group=factor(variant))) +
    geom_point(aes(shape=factor(variant)), size=0.5, alpha=0.5) +
    # include everything in one dataframe, outofmemory case as shape
    geom_point(data=outofmemory, aes(x=txacc,y=tx, color=factor(variant), group=factor(variant)), shape = 4, size=5) +
    scale_size_identity(name=NULL) +
    scale_shape_discrete(name=NULL, labels=labelByVariant, breaks=breakByVariant) +
    scale_color_manual(name =NULL, labels=labelByVariant, values=colorByVariant, breaks=breakByVariant) +
    labs(x='Processed M Transactions [txn]', y = 'TPC-C throughput [txns/sec]') +
    geom_smooth(method ="auto", size=0.5, se=FALSE) +
#    geom_line() +
    theme_bw() +
    theme(legend.position = 'top') +
    expand_limits(y=0, x=0) +
   facet_grid(row=vars(c_worker_threads), scales="free")#geom_point(data=outofmemory, aes(x=t,y=tx, colour=factor(variant)), shape =4, size= 10)
print(tx)

CairoPDF("./tpcc_C.pdf", bg="transparent")
print(tx)
dev.off()



gdc=sqldf("select max(txacc) txacc,variant, c_worker_threads from acc group by variant, c_worker_threads")
sqldf("select min(txacc) from gdc")
