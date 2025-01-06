/*
 * Copyright (c) 2019 - 2025 Geode-solutions
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 */

#pragma once

#include <string>
#include <vector>

#include <absl/container/flat_hash_map.h>

#include <geode/basic/logger.hpp>
#include <geode/basic/types.hpp>
#include <geode/basic/uuid.hpp>

#include <geode/inspector/common.hpp>

namespace geode
{
    template < typename IssueType >
    class InspectionIssues
    {
    public:
        explicit InspectionIssues( std::string_view issue_description )
            : description_{ issue_description }
        {
        }

        InspectionIssues() = default;

        void set_description( std::string_view issue_description )
        {
            description_ = to_string( issue_description );
        }

        [[nodiscard]] index_t nb_issues() const
        {
            return issues_.size();
        }

        void add_issue( IssueType issue, std::string message )
        {
            issues_.emplace_back( std::move( issue ) );
            messages_.emplace_back( std::move( message ) );
        }

        [[nodiscard]] std::string_view description() const
        {
            return description_;
        }

        [[nodiscard]] std::string string() const
        {
            if( issues_.empty() )
            {
                return absl::StrCat( description_, " -> No Issues :)" );
            }
            auto message = absl::StrCat( description_ );
            for( const auto& issue : messages_ )
            {
                absl::StrAppend( &message, "\n ->    ", issue );
            }
            return message;
        }

        [[nodiscard]] const std::vector< IssueType >& issues() const
        {
            return issues_;
        }

    private:
        std::string description_{
            "Default inspection issue message. This message "
            "should have been overriden."
        };
        std::vector< IssueType > issues_{};
        std::vector< std::string > messages_{};
    };

    template < typename IssueType >
    class InspectionIssuesMap
    {
    public:
        explicit InspectionIssuesMap( std::string_view issue_description )
            : description_{ issue_description }
        {
        }

        InspectionIssuesMap() = default;

        void set_description( std::string_view issue_description )
        {
            description_ = to_string( issue_description );
        }

        [[nodiscard]] std::string_view description() const
        {
            return description_;
        }

        [[nodiscard]] index_t nb_issues() const
        {
            return issues_map_.size();
        }

        void add_issues_to_map(
            const uuid& id_to_issues, InspectionIssues< IssueType > issues )
        {
            if( issues.nb_issues() == 0 )
            {
                return;
            }
            issues_map_.emplace( id_to_issues, std::move( issues ) );
        }

        [[nodiscard]] std::string string() const
        {
            if( issues_map_.empty() )
            {
                return absl::StrCat( description_, " -> No Issues :)" );
            }
            auto message = absl::StrCat( description_ );
            for( const auto& issues : issues_map_ )
            {
                absl::StrAppend( &message, "\n ->  ", issues.second.string() );
            }
            return message;
        }

        [[nodiscard]] const absl::flat_hash_map< uuid,
            InspectionIssues< IssueType > >&
            issues_map() const
        {
            return issues_map_;
        }

    private:
        std::string description_{
            "Default inspection issue message. This message "
            "should have been overriden."
        };
        absl::flat_hash_map< uuid, InspectionIssues< IssueType > > issues_map_;
    };
} // namespace geode