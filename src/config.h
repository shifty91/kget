#ifndef _CONFIG_H_
#define _CONFIG_H_

class Config
{
private:
    static Config *m_instance;
    bool m_show_pg;
    bool m_follow_redirects;
    bool m_verify_peer;

public:
    Config() :
        m_show_pg{true}, m_follow_redirects{true}, m_verify_peer{false}
    {}

    ~Config()
    {
        delete m_instance;
    }

    static Config *instance()
    {
        if (m_instance == nullptr)
            m_instance = new Config();
        return m_instance;
    }

    inline const bool& show_pg() const
    {
        return m_show_pg;
    }

    inline bool& show_pg()
    {
        return m_show_pg;
    }

    inline const bool& follow_redirects() const
    {
        return m_follow_redirects;
    }

    inline bool& follow_redirects()
    {
        return m_follow_redirects;
    }

    inline const bool& verify_peer() const
    {
        return m_verify_peer;
    }

    inline bool& verify_peer()
    {
        return m_verify_peer;
    }
};

#endif /* _CONFIG_H_ */
