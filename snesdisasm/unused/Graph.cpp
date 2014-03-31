#include <vector>
#include <set>
#include <tbb/tbb.h>
#include <algorithm>
#include <iostream>

//http://paste.kde.org/pf7eed4a8/

struct Domain {
    unsigned int w, h;
};

class Graph {
    struct Pos {
        float x, y;
    };
public:
    Graph() {};
	
    void print(const Domain &domain) {
        for (unsigned int y = 0; y <= domain.h; ++y) {
            for (unsigned int x = 0; x <= domain.w; ++x) {
                bool found = false;
                for (Pos & n : m_nodes) {
                    if (n.x >= x && n.x < x + 1 && n.y >= y && n.y < y + 1) {
                        found = true;
                    }
                }
                if (found) {
                    std::cout << "# ";
                } else {
                    std::cout << ". ";
                }
            }
            std::cout << std::endl;
        }
        std::cout << std::endl;
    }
	
    unsigned int addNode(float x, float y) {
        m_nodes.resize(m_nodes.size() + 1);
        m_nodes.back().x = x;
        m_nodes.back().y = y;
        m_adj.resize(m_adj.size() + 1);
        return m_nodes.size() - 1;
    }

    void connectNodes(unsigned int a, unsigned int b) {
        m_adj[a].insert(b);
        m_adj[b].insert(a);
    }

    void align(const Domain &domain, unsigned int steps) {
        std::vector<Pos> acc;
        acc.resize(m_nodes.size());

        auto pull_together = [&](const tbb::blocked_range<size_t> &range) {
            for (int i = range.begin(); i != range.end(); ++i) {
                acc[i].x = m_nodes[i].x;
                acc[i].y = m_nodes[i].y;
                for (unsigned int j : m_adj[i]) {
                    //check all connected nodes
                    float dx = m_nodes[j].x - m_nodes[i].x;
                    float dy = m_nodes[j].y - m_nodes[i].y;
                    float lenSq = dx * dx + dy * dy;
                    if (lenSq == 0) {
                        continue;
                    }
                    float len = sqrt(lenSq);
                    //attract one 1 unit to the other
                    acc[i].x += dx / len;
                    acc[i].y += dy / len;
                }
            }
        };

        auto push_apart = [&](const tbb::blocked_range<size_t> &range) {
            for (int i = range.begin(); i != range.end(); ++i) {
                acc[i].x = m_nodes[i].x;
                acc[i].y = m_nodes[i].y;
                for (int j = 0; j < m_nodes.size(); ++j) {
                    if (i == j) {
                        continue;
                    }
                    float dx = m_nodes[j].x - m_nodes[i].x;
                    float dy = m_nodes[j].y - m_nodes[i].y;
                    float lenSq = dx * dx + dy * dy;
                    if (lenSq == 0) {
                        continue;
                    }
                    float len = sqrt(lenSq);
                    //move appart; more, if they are closer
                    acc[i].x -= dx / lenSq;
                    acc[i].y -= dy / lenSq;
                }
            }
        };

        for (unsigned int i = 0; i < steps; ++i) {
            tbb::parallel_for(tbb::blocked_range<size_t> (0, m_nodes.size()), pull_together);
            std::swap(acc, m_nodes);
            tbb::parallel_for(tbb::blocked_range<size_t> (0, m_nodes.size()), push_apart);
            std::swap(acc, m_nodes);
        }

        if(m_nodes.size() >= 1){
            Pos min{m_nodes[0].x, m_nodes[0].y};
            Pos max{m_nodes[0].x, acc[0].y};

            for(const Pos& p: m_nodes){
                if(p.x < min.x) min.x = p.x;
                if(p.y < min.y) min.y = p.y;
                if(p.x > max.x) max.x = p.x;
                if(p.y > max.y) max.y = p.y;
            }

            float scaleX = float(domain.w)/(max.x - min.y);
            float scaleY = float(domain.h)/(max.y - min.y);
            float offsetX = min.x * scaleX;
            float offsetY = min.y * scaleY;

            for(Pos& p: m_nodes){
                p.x = p.x * scaleX - offsetX;
                p.y = p.y * scaleY - offsetY;
            }
        }
    }

private:
    std::vector<Pos> m_nodes;
    std::vector<std::set<unsigned int>> m_adj;
};