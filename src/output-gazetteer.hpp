#ifndef OSM2PGSQL_OUTPUT_GAZETTEER_HPP
#define OSM2PGSQL_OUTPUT_GAZETTEER_HPP

/**
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * This file is part of osm2pgsql (https://osm2pgsql.org/).
 *
 * Copyright (C) 2006-2023 by the osm2pgsql developer community.
 * For a full list of authors see the git log.
 */

#include <memory>
#include <utility>

#include <osmium/memory/buffer.hpp>

#include "gazetteer-style.hpp"
#include "osmtypes.hpp"
#include "output.hpp"
#include "reprojection.hpp"
#include "wkb.hpp"

class db_copy_thread_t;
class thread_pool_t;

struct middle_query_t;

class output_gazetteer_t : public output_t
{
public:
    /// Constructor for new objects
    output_gazetteer_t(std::shared_ptr<middle_query_t> const &mid,
                       std::shared_ptr<thread_pool_t> thread_pool,
                       options_t const &options)
    : output_t(mid, std::move(thread_pool), options),
      m_copy(std::make_shared<db_copy_thread_t>(options.conninfo)),
      m_proj(options.projection)
    {
        m_style.load_style(options.style);
    }

    /// Constructor for cloned objects
    output_gazetteer_t(output_gazetteer_t const *other,
                       std::shared_ptr<middle_query_t> const &mid,
                       std::shared_ptr<db_copy_thread_t> const &copy_thread)
    : output_t(other, mid), m_copy(copy_thread),
      m_proj(other->get_options()->projection)
    {}

    output_gazetteer_t(output_gazetteer_t const &) = delete;
    output_gazetteer_t &operator=(output_gazetteer_t const &) = delete;

    output_gazetteer_t(output_gazetteer_t &&) = delete;
    output_gazetteer_t &operator=(output_gazetteer_t &&) = delete;

    ~output_gazetteer_t() override;

    std::shared_ptr<output_t>
    clone(std::shared_ptr<middle_query_t> const &mid,
          std::shared_ptr<db_copy_thread_t> const &copy_thread) const override
    {
        return std::make_shared<output_gazetteer_t>(this, mid, copy_thread);
    }

    void start() override;
    void stop() noexcept override {}
    void sync() override;

    void pending_way(osmid_t) noexcept override {}
    void pending_relation(osmid_t) noexcept override {}

    void node_add(osmium::Node const &node) override;
    void way_add(osmium::Way *way) override;
    void relation_add(osmium::Relation const &rel) override;

    void node_modify(osmium::Node const &node) override;
    void way_modify(osmium::Way *way) override;
    void relation_modify(osmium::Relation const &rel) override;

    void node_delete(osmid_t id) override { delete_unused_full('N', id); }
    void way_delete(osmid_t id) override { delete_unused_full('W', id); }
    void relation_delete(osmid_t id) override { delete_unused_full('R', id); }

private:
    enum
    {
        PLACE_BUFFER_SIZE = 4096
    };

    /// Delete all places that are not covered by the current style results.
    void delete_unused_classes(char osm_type, osmid_t osm_id);
    /// Delete all places for the given OSM object.
    void delete_unused_full(char osm_type, osmid_t osm_id);
    bool process_node(osmium::Node const &node);
    bool process_way(osmium::Way *way);
    bool process_relation(osmium::Relation const &rel);

    gazetteer_copy_mgr_t m_copy;
    gazetteer_style_t m_style;

    std::shared_ptr<reprojection> m_proj;
    osmium::memory::Buffer m_osmium_buffer{
        PLACE_BUFFER_SIZE, osmium::memory::Buffer::auto_grow::yes};
};

#endif // OSM2PGSQL_OUTPUT_GAZETTEER_HPP
