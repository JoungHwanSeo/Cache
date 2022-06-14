/**
 * ECE 430.322: Computer Organization 
 * Lab 4: Cache Simulation
 */

#include "cache.h"
#include <iostream>
#include <string>
#include <cmath>

/**
 * This allocates an "assoc" number of cache entries per a set
 * @param assoc - number of cache entries in a set
 */
cache_set_c::cache_set_c(int assoc) {
  m_entry = new cache_entry_c[assoc];
  m_assoc = assoc;
}

// cache_set_c destructor
cache_set_c::~cache_set_c() {
  delete[] m_entry;
}

/**
 * This constructor initializes a cache structure based on the cache parameters.
 * @param name - cache name; use any name you want
 * @param num_sets - number of sets in a cache
 * @param assoc - number of cache entries in a set
 * @param line_size - cache block (line) size in bytes
 */
cache_c::cache_c(std::string name, int num_sets, int assoc, int line_size) {
  m_name = name;
  m_num_sets = num_sets;
  m_line_size = line_size;

  m_set = new cache_set_c *[m_num_sets];

  for (int ii = 0; ii < m_num_sets; ++ii) {
    m_set[ii] = new cache_set_c(assoc);
    //LRU logic �����ϸ鼭 cnrk
    m_set[ii]->MRU = 0; //�� ó���� �׳� 0��° way�� MRU�� �ʱ�ȭ...

    // initialize tag/valid/dirty bits
    for (int jj = 0; jj < assoc; ++jj) {
      m_set[ii]->m_entry[jj].m_valid = false;
      m_set[ii]->m_entry[jj].m_dirty = false;
      m_set[ii]->m_entry[jj].m_tag   = 0;
    }
  }

  // initialize stats
  m_num_accesses = 0;
  m_num_hits = 0;
  m_num_misses = 0;
  m_num_writes = 0;
  m_num_writebacks = 0;

  //���������� �߰���
  block_bit = 0;
  idx_bit = 0;
  tag_bit = 0;

  int tmp_num_set = num_sets;
  int tmp_line_size = line_size;
  //idx�� bit�� block_bit�� size���� (log_2)
  while (1) {
      tmp_num_set = tmp_num_set / 2;
      if (tmp_num_set)
          idx_bit++;
      else
          break;
  }

  while (1) {
      tmp_line_siz = tmp_line_size / 2;
      if (tmp_line_size)
          block_bit++;
      else
          break;
  }
  tag_bit = 32 - idx_bit - block_bit;
}

// cache_c destructor
cache_c::~cache_c() {
  for (int ii = 0; ii < m_num_sets; ++ii) { delete m_set[ii]; }
  delete[] m_set;
}

/** 
 * This function looks up in the cache for a memory reference.
 * This needs to update all the necessary meta-data (e.g., tag/valid/dirty) 
 * and the cache statistics, depending on a cache hit or a miss.
 * @param address - memory address 
 * @param access_type - read (0), write (1), or instruction fetch (2)
 */
void cache_c::access(addr_t address, int access_type) {
  ////////////////////////////////////////////////////////////////////
  // TODO: Write the code to implement this function
    int idx_block = address % (m_num_sets * m_line_size);
    int idx = idx_block / m_line_size;
    int tag = address / (m_num_sets * m_line_size);

    cache_entry_c entry;

    bool hit = false;

    m_num_accesses++; //�����ϱ⸸ �ϸ� �ϴ� +1

    //���� ����
    switch (access_type) {
    case 0: // read
        m_num_reads++; //�ϴ� read�̹Ƿ� +1
        for (int i = 0; i < m_num_sets; i++) {
            entry = m_set[idx]->m_entry[i];
            if (entry.m_valid && entry.m_tag == tag) {
                m_num_hit++; 
                hit = true;
                break;
            }
        }
        if (hit == false) {
            m_num_miss++; //��� set�� ã�Ҵµ��� tag match ����
            //�� read miss
            //�� ��� �ش� set�� data�������� ���̹Ƿ� way�� �ϳ��� valid�Ǿ����

            ///assoc = 1�ΰ��... Part 2 ���� ���ľ���!!!!!!!!!!!!!1

            entry.m_valid = true;
        }
        break;

    case 1: //write
        m_num_writes++; //�ϴ� write�̹Ƿ�
        for (int i = 0; i < m_num_sets; i++) {
            entry = m_set[idx]->m_entry[i];
            if (entry.m_valid && entry.m_tag == tag) {
                m_num_hit++;
                m_num_writebacks++; 
                entry.m_dirty = true; //write hit���� write back�Ͽ� dirty
                hit = true;
                break;
            }
        }
        if (hit == false) {
            //�ش� idx�� ��� assoã�Ƶ� ������ -> write allocate
        }
        break;
    case 2: //instruction fetch
        m_num_reads++; // instruction fetch�� �ϴ� read��
        break;
    }
  ////////////////////////////////////////////////////////////////////
}

/**
 * Print statistics (DO NOT CHANGE)
 */
void cache_c::print_stats() {
  std::cout << "------------------------------" << "\n";
  std::cout << m_name << " Hit Rate: "          << (double)m_num_hits/m_num_accesses*100 << " % \n";
  std::cout << "------------------------------" << "\n";
  std::cout << "number of accesses: "    << m_num_accesses << "\n";
  std::cout << "number of hits: "        << m_num_hits << "\n";
  std::cout << "number of misses: "      << m_num_misses << "\n";
  std::cout << "number of writes: "      << m_num_writes << "\n";
  std::cout << "number of writebacks: "  << m_num_writebacks << "\n";
}

