options(show.error.locations=TRUE)
setwd(dirname(sys.frame(1)$ofile))
rm(list = ls())
graphics.off()

csv_data = read.csv(file = "bs-timing-phys.csv")

bs4k   = subset(csv_data, size == 4096)
bs8k   = subset(csv_data, size == 8192)
bs16k  = subset(csv_data, size == 16384)
bs32k  = subset(csv_data, size == 32768)
bs64k  = subset(csv_data, size == 65536)
bs128k = subset(csv_data, size == 131072)
bs256k = subset(csv_data, size == 262144)
bs512k = subset(csv_data, size == 524288)
bs1m   = subset(csv_data, size == 1048576)
bs2m   = subset(csv_data, size == 2097152)

bp = boxplot(
	bs4k$block.time * 1000,
	bs8k$block.time * 1000,
	bs16k$block.time * 1000,
	bs32k$block.time * 1000,
	bs64k$block.time * 1000,
	bs128k$block.time * 1000,
	bs256k$block.time * 1000,
	bs512k$block.time * 1000,
	bs1m$block.time * 1000,
	bs2m$block.time * 1000,
	ylab="Time (ms)", axes=FALSE, xlab="Block size (bytes)", col="grey93")

yticks = seq(max(0, round(par("usr")[3]*2)/2), round(par("usr")[4]*2)/2, 0.02)
abline(h=yticks, v=seq(1, 10), col="grey80", lty=3)
axis(1, seq(1, 10), labels=c("4k", "8k", "16k", "32k", "64k", "128k", "256k", "512k", "1m", "2m"))
axis(2, yticks, las=2)
title("Block Size Read Timing (physical addressing)")

# plot(csv_data$size, csv_data$block.time, log="x")
