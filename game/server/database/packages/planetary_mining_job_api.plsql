create or replace package body planetary_mining_job_api
as
	procedure lock_account(p_station_id number)
	as
		v_station_id accounts.station_id%type;
	begin
		select station_id
		into v_station_id
		from accounts
		where station_id = p_station_id
		for update;
	end;

	function is_account_character(p_station_id number, p_character_id number) return boolean
	as
		v_character_count integer;
	begin
		select count(*)
		into v_character_count
		from players
		where character_object = p_character_id
		and station_id = p_station_id;

		return v_character_count = 1;
	end;

	function get_job_count(p_galaxy_id varchar2, p_station_id number) return integer
	as
		v_job_count integer;
	begin
		select count(*)
		into v_job_count
		from planetary_mining_jobs
		where galaxy_id = p_galaxy_id
		and station_id = p_station_id;

		return v_job_count;
	end;

	procedure reserve_job
	(
		p_galaxy_id varchar2,
		p_station_id number,
		p_character_id number,
		p_job_sequence number,
		p_result out number,
		p_job_count out number
	)
	as
		v_existing_sequence planetary_mining_jobs.job_sequence%type;
	begin
		savepoint pmd_reserve_job;
		p_result := result_database_error;
		p_job_count := -1;

		lock_account(p_station_id);
		if not is_account_character(p_station_id, p_character_id) then
			p_result := result_invalid_character;
			return;
		end if;

		delete from planetary_mining_jobs
		where galaxy_id = p_galaxy_id
		and character_id = p_character_id
		and station_id <> p_station_id;

		delete from planetary_mining_jobs jobs
		where jobs.galaxy_id = p_galaxy_id
		and jobs.station_id = p_station_id
		and not exists
		(
			select 1
			from players
			where character_object = jobs.character_id
			and station_id = jobs.station_id
		);

		begin
			select job_sequence
			into v_existing_sequence
			from planetary_mining_jobs
			where galaxy_id = p_galaxy_id
			and character_id = p_character_id;

			p_job_count := get_job_count(p_galaxy_id, p_station_id);
			if v_existing_sequence = p_job_sequence then
				p_result := result_success;
			else
				p_result := result_character_conflict;
			end if;
			return;
		exception
			when no_data_found then
				null;
		end;

		p_job_count := get_job_count(p_galaxy_id, p_station_id);
		if p_job_count >= 3 then
			p_result := result_limit_reached;
			return;
		end if;

		insert into planetary_mining_jobs
		(
			galaxy_id,
			station_id,
			character_id,
			job_sequence,
			reserved_at
		)
		values
		(
			p_galaxy_id,
			p_station_id,
			p_character_id,
			p_job_sequence,
			sysdate
		);

		p_job_count := p_job_count + 1;
		p_result := result_success;
	exception
		when no_data_found then
			rollback to pmd_reserve_job;
			p_result := result_invalid_character;
			p_job_count := -1;
		when others then
			rollback to pmd_reserve_job;
			p_result := result_database_error;
			p_job_count := -1;
	end;

	procedure release_job
	(
		p_galaxy_id varchar2,
		p_station_id number,
		p_character_id number,
		p_job_sequence number,
		p_result out number,
		p_job_count out number
	)
	as
	begin
		savepoint pmd_release_job;
		p_result := result_database_error;
		p_job_count := -1;

		lock_account(p_station_id);
		delete from planetary_mining_jobs
		where galaxy_id = p_galaxy_id
		and character_id = p_character_id
		and station_id <> p_station_id;

		delete from planetary_mining_jobs
		where galaxy_id = p_galaxy_id
		and station_id = p_station_id
		and character_id = p_character_id
		and job_sequence = p_job_sequence;

		p_job_count := get_job_count(p_galaxy_id, p_station_id);
		p_result := result_success;
	exception
		when no_data_found then
			rollback to pmd_release_job;
			p_result := result_invalid_character;
			p_job_count := -1;
		when others then
			rollback to pmd_release_job;
			p_result := result_database_error;
			p_job_count := -1;
	end;
end;
/
