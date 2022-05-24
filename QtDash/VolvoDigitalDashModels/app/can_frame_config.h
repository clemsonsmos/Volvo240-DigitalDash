#ifndef CAN_FRAME_CONFIG_H
#define CAN_FRAME_CONFIG_H

#include <QtSerialBus>
#include <QString>
#include <QDataStream>

class CanFrameConfig {
public:
    enum class OperationType {
        MULTIPLY,
        DIVIDE,
        ADD
    };

    typedef struct Operation {
        OperationType type;
        qreal value;
    } Operation_t;

    CanFrameConfig(uint16_t frameId, uint8_t offset,
                   uint8_t size, bool sign, QString units) :
        mFrameId(frameId), mOffset(offset), mSize(size), mSigned(sign),
        mUnits(units) {

    }

    int addOperation(OperationType type, qreal value) {
        Operation_t op;
        op.type = type;
        op.value = value;

        mOperations.append(op);

        return mOperations.length();
    }

    qreal getValue(QByteArray payload) {
        QDataStream ds(payload);
        qreal ret = 0;
        // move to offset (minus 1?)
        if (ds.skipRawData(mOffset) < 0) {
            return qQNaN();
        }

        // get value
        switch (mSize) {
        case 1: {
            if (mSigned) {
                int8_t value;
                ds >> value;
                ret = (qreal)value;
            } else {
                uint8_t value;
                ds >> value;
                ret = (qreal)value;
            }
        }
            break;
        case 2: {
            if (mSigned) {
                int16_t value;
                ds >> value;
                ret = (qreal)value;
            } else {
                uint16_t value;
                ds >> value;
                ret = (qreal)value;
            }
        }
            break;
        case 4:{
            if (mSigned) {
                int32_t value;
                ds >> value;
                ret = (qreal)value;
            } else {
                uint32_t value;
                ds >> value;
                ret = (qreal)value;
            }
        }
            break;
        case 8:{
            if (mSigned) {
                int64_t value;
                ds >> value;
                ret = (qreal)value;
            } else {
                uint64_t value;
                ds >> value;
                ret = (qreal)value;
            }
        }
            break;
        }

        for (Operation_t ops : mOperations) {
            switch ((int)ops.type) {
            case (int)OperationType::MULTIPLY:
                ret *= ops.value;
                break;
            case (int) OperationType::DIVIDE:
                ret /= ops.value;
                break;
            case (int) OperationType::ADD:
                ret += ops.value;
                break;
            }
        }

        return ret;
    }

private:
    uint16_t mFrameId;
    uint8_t mOffset;
    uint8_t mSize;
    bool mSigned;
    QString mName;
    QString mUnits;
    QVector<Operation_t> mOperations;
};

#endif // CAN_FRAME_CONFIG_H
