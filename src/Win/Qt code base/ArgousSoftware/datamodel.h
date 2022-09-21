#ifndef DATAMODEL_H
#define DATAMODEL_H

#include <QDialog>

namespace Ui {
class DataModel;
}

class DataModel : public QDialog
{
    Q_OBJECT

public:
    explicit DataModel(QWidget *parent = nullptr);
    ~DataModel();

private:
    Ui::DataModel *ui;
};

#endif // DATAMODEL_H
