#include <iostream>
#include <memory>
#include <list>

using namespace std;

class ObserverBoardI
{
public:
    virtual void update(float a, float b, float c) = 0;
};

class DisplayBoardI
{
public:
    virtual void show() = 0;
};

class WeatherDataI
{
public:
    virtual void registerOb(shared_ptr<ObserverBoardI> pOb) = 0;
    virtual void removeOb(shared_ptr<ObserverBoardI> pOb) = 0;
    virtual void notifyOb() = 0;
};

class ParaWeatherData: public WeatherDataI
{
public:
    void sensorDataChange(float a, float b, float c)
    {
        m_humidity = a;
        m_temperature = b;
        m_pressure = c;
        notifyOb();
    }

    void registerOb(shared_ptr<ObserverBoardI> pOb)
    {
        m_obs.push_back(pOb);
    }

    void removeOb(shared_ptr<ObserverBoardI> pOb)
    {
        m_obs.remove(pOb);
    }

private:
    void notifyOb()
    {
        auto it = m_obs.begin();
        auto end = m_obs.end();
        while(it != end)
        {
            (*it)->update(m_humidity, m_temperature, m_pressure);
            dynamic_pointer_cast< DisplayBoardI >(*it)->show();
            ++it;
        }
    }

    float m_humidity;
    float m_temperature;
    float m_pressure;
    list< shared_ptr<ObserverBoardI> > m_obs;
};

class CurrentConditionBoard : public ObserverBoardI, public DisplayBoardI, public enable_shared_from_this<CurrentConditionBoard>
{
public:
    CurrentConditionBoard(ParaWeatherData& a) : m_data(a)
    {
    }
    void show()
    {
        cout<<"_____CurrentConditionBoard_____"<<endl;
        cout<<"humidity: "<<m_h<<endl;
        cout<<"temperature: "<<m_t<<endl;
        cout<<"pressure: "<<m_p<<endl;
        cout<<"_______________________________"<<endl;
    }
    void update(float h, float t, float p)
    {
        m_h = h;
        m_t = t;
        m_p = p;
    }
private:
    float m_h;
    float m_t;
    float m_p;
    ParaWeatherData& m_data;
};

class StatisticBoard : public ObserverBoardI, public DisplayBoardI, public enable_shared_from_this<StatisticBoard>
{
public:
    StatisticBoard(ParaWeatherData& a)
        : m_maxt(-1000), m_mint(1000), m_avet(0), m_count(0), m_data(a)
    {
    }
    void show()
    {
        cout<<"________StatisticBoard_________"<<endl;
        cout<<"lowest  temperature: "<<m_mint<<endl;
        cout<<"highest temperature: "<<m_maxt<<endl;
        cout<<"average temperature: "<<m_avet<<endl;
        cout<<"_______________________________"<<endl;
    }
    void update(float h, float t, float p)
    {
        ++m_count;
        if (t > m_maxt)
            m_maxt = t;
        if (t < m_mint)
            m_mint = t;
        m_avet = (m_avet * (m_count - 1) + t) / m_count;
    }
private:
    float m_maxt;
    float m_mint;
    float m_avet;
    int m_count;
    ParaWeatherData& m_data;
};

int main()
{
    shared_ptr<ParaWeatherData> pData(make_shared<ParaWeatherData>());
    shared_ptr<CurrentConditionBoard> pCurrentB(make_shared<CurrentConditionBoard>(*pData));
    shared_ptr<StatisticBoard> pStatisticB(make_shared<StatisticBoard>(*pData));

    // register observers
    pData->registerOb(pCurrentB);
    pData->registerOb(pStatisticB);

    // data changed and notify observers 
    pData->sensorDataChange(10.2, 28.2, 1001);
    pData->sensorDataChange(12, 30.12, 1003);
    
    cout << "\nremove observer: CurrentConditionBoard\n\n";
    pData->removeOb(pCurrentB);

    // notify remains observers
    pData->sensorDataChange(100, 40, 1900);

    return 0;
}
