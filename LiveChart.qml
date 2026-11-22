import QtQuick

Canvas {
    id: chart
    property var whiteSeries: []
    property var blackSeries: []
    onWhiteSeriesChanged: requestPaint()
    onBlackSeriesChanged: requestPaint()
    onWidthChanged: requestPaint()
    onHeightChanged: requestPaint()

    onPaint: {
        const ctx = getContext("2d")
        ctx.clearRect(0, 0, width, height)
        const left = 28, right = width - 8, top = 9, bottom = height - 12
        ctx.font = "10px sans-serif"
        ctx.fillStyle = "#c6bad7"
        ctx.strokeStyle = "#323c57"
        ctx.lineWidth = 1
        for (const value of [0, 50, 100]) {
            const y = bottom - (bottom - top) * value / 100
            ctx.beginPath()
            ctx.moveTo(left, y)
            ctx.lineTo(right, y)
            ctx.stroke()
            ctx.fillText(String(value), 2, y + 3)
        }
        const span = Math.max(1, whiteSeries.length - 1, blackSeries.length - 1)
        function draw(series, color) {
            ctx.strokeStyle = color
            ctx.fillStyle = color
            ctx.lineWidth = 2
            ctx.beginPath()
            for (let i = 0; i < series.length; ++i) {
                const x = left + (right - left) * i / span
                const y = bottom - (bottom - top) * series[i] / 100
                if (i === 0) ctx.moveTo(x, y)
                else ctx.lineTo(x, y)
            }
            ctx.stroke()
            if (series.length === 1) {
                ctx.beginPath()
                ctx.arc(left, bottom - (bottom - top) * series[0] / 100, 3, 0, 2 * Math.PI)
                ctx.fill()
            }
        }
        draw(whiteSeries, "#45f4ff")
        draw(blackSeries, "#ff5bc7")
    }
}
