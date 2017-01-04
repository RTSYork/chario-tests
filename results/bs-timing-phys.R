options(show.error.locations=TRUE)
setwd(dirname(sys.frame(1)$ofile))
rm(list = ls())
graphics.off()

csv_data = read.csv(file = "bs-timing-phys.csv")

plot(csv_data$size, csv_data$block.time, log="x")
