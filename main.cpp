#include <QtWidgets/QApplication>
#include <QtWidgets/QMainWindow>
#include <QtCharts/QChartView>
#include <QtCharts/QLineSeries>
#include <QtCharts/QValueAxis>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QLabel>
#include <QtWidgets/QToolTip>
#include <QtGui/QMouseEvent>
#include <cmath>
#include <algorithm>

QT_CHARTS_USE_NAMESPACE

class InteractiveView : public QChartView {
public:
    InteractiveView(QChart *chart, int *N) : QChartView(chart), m_N(N) {
        setMouseTracking(true);
        m_marker = new QGraphicsEllipseItem(-10, -10, 20, 20, chart);
        m_marker->setBrush(QColor(52, 152, 219));
    }
protected:
    void mouseMoveEvent(QMouseEvent *event) override {
        QPointF val = chart()->mapToValue(event->pos());
        double x = std::clamp((double)val.x(), 0.0, 5.0);
        double p = std::exp(-x);

        QPointF pos = chart()->mapToPosition(QPointF(x, p));
        m_marker->setPos(pos);

        int N = *m_N;
        double others_avg = (1.0 - p) / (double)(N - 1);

        QString tip = QString("<div style='font-size:24px;'>"
                              "<b>Loss:</b> %1<br>"
                              "<b>Target Prob:</b> %2<br>"
                              "<b>Others Avg Prob:</b> %3</div>")
            .arg(QString::number(x, 'f', 4))
            .arg(QString::number(p, 'f', 4))
            .arg(QString::number(others_avg, 'f', 4));

        QToolTip::showText(event->globalPos(), tip);
        QChartView::mouseMoveEvent(event);
    }
private:
    QGraphicsEllipseItem *m_marker;
    int *m_N;
};

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    MainWindow() {
        m_numClasses = 2;
        QWidget *widget = new QWidget();
        QVBoxLayout *layout = new QVBoxLayout(widget);

        QLabel *formulaHeader = new QLabel(
            "<b>Loss Basis:</b> L = -ln(P<sub>target</sub>) &nbsp;&nbsp;&nbsp; "
            "<b>Average of Others:</b> P<sub>others_avg</sub> = (1 - P<sub>target</sub>) / (N - 1)"
        );
        formulaHeader->setStyleSheet("font-size: 24px; padding: 25px; background: #fdfdfd; border: 1px solid #ddd; border-radius: 10px;");
        layout->addWidget(formulaHeader);

        QHBoxLayout *header = new QHBoxLayout();
        QLabel *label = new QLabel("Classes (N):");
        label->setStyleSheet("font-weight: bold;");

        QSpinBox *spin = new QSpinBox();
        spin->setRange(2, 100000);
        spin->setValue(m_numClasses);
        spin->setMinimumHeight(60);
        spin->setMinimumWidth(200);

        header->addWidget(label);
        header->addWidget(spin);
        header->addStretch();
        layout->addLayout(header);

        m_chart = new QChart();
        m_chart->legend()->hide();
        m_chart->setTitle("Cross-Entropy Analysis");
        m_chart->setTitleFont(QFont("Arial", 24, QFont::Bold));

        QLineSeries *curve = new QLineSeries();
        for (float x = 0; x <= 5.0f; x += 0.01f) curve->append(x, std::exp(-x));
        m_chart->addSeries(curve);

        double p_refs[] = {0.9, 0.8, 0.7};
        for (double p : p_refs) {
            QLineSeries *s = new QLineSeries();
            double lx = -std::log(p);
            s->append(lx, 0); s->append(lx, 1);
            s->setPen(QPen(Qt::gray, 2, Qt::DashLine));
            m_chart->addSeries(s);
        }

        m_randomLine = new QLineSeries();
        m_randomLine->setPen(QPen(Qt::red, 4, Qt::DashLine));
        m_chart->addSeries(m_randomLine);

        m_chart->createDefaultAxes();

        QValueAxis *axisX = qobject_cast<QValueAxis*>(m_chart->axes(Qt::Horizontal).at(0));
        QValueAxis *axisY = qobject_cast<QValueAxis*>(m_chart->axes(Qt::Vertical).at(0));

        QFont axisLabelFont("Arial", 16);
        QFont axisTitleFont("Arial", 18, QFont::Bold);

        axisX->setLabelsFont(axisLabelFont);
        axisY->setLabelsFont(axisLabelFont);
        axisX->setTitleFont(axisTitleFont);
        axisY->setTitleFont(axisTitleFont);

        axisX->setRange(0, 5);
        axisY->setRange(0, 1);
        axisX->setTitleText("Loss");
        axisY->setTitleText("Probability");

        updateRandom(m_numClasses);

        InteractiveView *view = new InteractiveView(m_chart, &m_numClasses);
        view->setRenderHint(QPainter::Antialiasing);
        layout->addWidget(view);

        connect(spin, QOverload<int>::of(&QSpinBox::valueChanged), this, &MainWindow::updateRandom);
        setCentralWidget(widget);
        resize(1400, 1000);
    }

private slots:
    void updateRandom(int n) {
        m_numClasses = n;
        double lx = -std::log(1.0 / n);
        m_randomLine->clear();
        m_randomLine->append(lx, 0);
        m_randomLine->append(lx, 1);
    }

private:
    QChart *m_chart;
    QLineSeries *m_randomLine;
    int m_numClasses;
};

int main(int argc, char *argv[]) {
    QCoreApplication::setAttribute(Qt::AA_Use96Dpi);
    QApplication a(argc, argv);

    QFont f = a.font();
    f.setPointSize(20);
    a.setFont(f);

    MainWindow w;
    w.show();
    return a.exec();
}

#include "main.moc"