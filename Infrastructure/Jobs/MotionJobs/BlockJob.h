/*! @file BlockJob.h
    @brief Declaration of BlockJob class.
 
    @class BlockJob
    @brief A class to encapsulate jobs issued for the walk module.
 
    @author Jason Kulk
 
  Copyright (c) 2009, 2010 Jason Kulk
 
    This file is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This file is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with NUbot.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef BLOCKJOB_H
#define BLOCKJOB_H

#include "../MotionJob.h"
#include <vector>


class BlockJob : public MotionJob
{
public:
    BlockJob(double time, float x, float y);
    BlockJob(double time, const std::vector<float>& position);
    BlockJob(double time, std::istream& input);
    ~BlockJob();
    
    void setPosition(double time, const std::vector<float>& newposition);
    void getPosition(double& time, std::vector<float>& position);
    
    virtual void summaryTo(std::ostream& output);
    virtual void csvTo(std::ostream& output);
    
    friend std::ostream& operator<<(std::ostream& output, const BlockJob& job);
    friend std::ostream& operator<<(std::ostream& output, const BlockJob* job);
protected:
    virtual void toStream(std::ostream& output) const;
private:
    std::vector<float> m_block_position;                 //!< the block position [x (cm), y (cm), theta (rad)]
};

#endif

