#include "mainwindow.h"
//添加两个头文件
#include <QVBoxLayout>


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    //修改窗口标题
    this->setWindowTitle("Tetris");
    //增加窗口图标
    setWindowIcon(QIcon(":/Tetris.ico"));

    ge = new gameEngine(ui->frm_main->x(), ui->frm_main->y(), ui->frm_next->x(), ui->frm_next->y());
    timer = new QTimer();
    timer->setInterval(SLOW_TIME_INTERVAL);
    connect(ui->btn_start, SIGNAL(clicked(bool)), this, SLOT(start_game()));
    connect(timer, SIGNAL(timeout()), this, SLOT(timer_start()));
    connect(ui->btn_new, SIGNAL(clicked(bool)), this, SLOT(new_game()));
    //增加help功能
    connect(ui->btn_help, SIGNAL(clicked(bool)), this, SLOT(help_game()));

    setFocusPolicy(Qt::StrongFocus);
    game_is_started = false;
}

void MainWindow::keyReleaseEvent(QKeyEvent* event)
{
    int key = event->key();

    if(game_is_started)
    {
        switch (key)
        {
        case Qt::Key_Down:
            timer->setInterval(SLOW_TIME_INTERVAL);
            break;

        case Qt::Key_Up:
            if(ge->get_move_down_ok())
            {
                ge->rotate();
                update();
            }
            break;

        default:
            break;
        }
    }
}

void MainWindow::keyPressEvent(QKeyEvent * event)
{
    if(ge->get_move_down_ok() && game_is_started)
    {
        int key = event->key();

        switch (key)
        {
        case Qt::Key_Left:
            ge->move_left();
            break;

        case Qt::Key_Right:
            ge->move_right();
            break;

        case Qt::Key_Down:
            if(timer->interval() != FAST_TIME_INTERVAL)
                timer->setInterval(FAST_TIME_INTERVAL);
            break;

        default:
            break;
        }
    }

    update();
}

void MainWindow::paintEvent(QPaintEvent *)
{
    int h = MAIN_BOARD_HEIGHT;
    int w = MAIN_BOARD_WIDTH;

    QPainter painter(this);

    for(int n = 0; n < NUM_OF_BOARDS; n++)
    {
        h = (n == MAIN_BOARD ? h : NEXT_BOARD_HEIGHT);
        w = (n == MAIN_BOARD ? w : NEXT_BOARD_WIDTH);

        for(int r = 0; r < h; r++)
        {
            for(int c = 0; c < w; c++)
            {
                //Draw a rectangle in the cell
                cell acell = (n == NEXT_BOARD ? ge->get_next_bd().get_a_cell(r, c) : ge->get_main_bd().get_a_cell(r, c));
                QPointF position = acell.get_position();
                QSizeF sizef(CELL_LENGTH, CELL_LENGTH);
                QRectF rectf(position, sizef);
                QColor color = acell.draw_cell_color();
                painter.fillRect(rectf, color);
            }
        }
    }
}

void MainWindow::timer_start()
{
    if(ge->get_move_down_ok())
    {
        ge->move_down();
    }
    else
    {
        if(ge->num_rows_cld() > 0)
        {
            int score = ge->compute_score();
            score += ui->lbl_score->text().toInt();
            ge->clear_rows();
            ge->rows_move_down();
            ui->lbl_score->setText(QString::number(score));
            //考虑当前分数是否比最大分数大，如果更大，修改最大分数
            if(score > max_score)
            {
                max_score = score;
                ui->max1->setText(QString::number(max_score));
            }
        }

        ge->main_place_a_shape();
        ge->get_next_bd().clear_board();
        ge->next_place_a_shape();
    }
    update();
}

void MainWindow::start_game()
{
    ge->next_place_a_shape();
    ge->main_place_a_shape();
    ge->get_next_bd().clear_board();
    ge->next_place_a_shape();
    update();
    ui->btn_start->setEnabled(false);
    timer->start();
    game_is_started = true;
}

void MainWindow::new_game()
{
    game_is_started = false;
    timer->stop();

    delete ge;
    ge = NULL;

    ge = new gameEngine(ui->frm_main->x(), ui->frm_main->y(), ui->frm_next->x(), ui->frm_next->y());

    ui->lbl_score->setText("0");
    //输出最大分数
    ui->max1->text();
    ui->btn_start->setEnabled(true);

    update();
}

//增加的help_game函数
void MainWindow::help_game()
{
    //新的帮助窗口
    QWidget *widget = new QWidget();
    widget->setWindowTitle(QObject::tr("help"));
    widget->resize(350,100);
    //增加窗口图标
    widget->setWindowIcon(QIcon(":/Tetris.ico"));
    //新建垂直布局管理器
    QVBoxLayout *layout=new QVBoxLayout;
    layout->setDirection(QBoxLayout::TopToBottom);
    //创建label
    QLabel *label1 = new QLabel();
    QLabel *label2 = new QLabel();
    QLabel *label3 = new QLabel();
    QLabel *label4 = new QLabel();
    label1->setText(QObject::tr("←:向左移动方块"));
    label2->setText(QObject::tr("→:向右移动方块"));
    label3->setText(QObject::tr("↑:旋转方块"));
    label4->setText(QObject::tr("↓:加快下降速度"));
    layout->addWidget(label1);
    layout->addWidget(label2);
    layout->addWidget(label3);
    layout->addWidget(label4);
    //将布局管理器添加到 widget 窗口中
    widget->setLayout(layout);
    //显示弹出窗口
    widget->show();
}


MainWindow::~MainWindow()
{
    delete ui;
    delete ge;
    ui = NULL;
    ge = NULL;
}
