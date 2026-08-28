create table planetary_mining_jobs
(
	galaxy_id varchar2(64),
	station_id int,
	character_id number(20),
	job_sequence int,
	reserved_at date default sysdate,
	constraint pk_pmd_jobs primary key (galaxy_id, character_id)
);

create index pmd_jobs_account_idx on planetary_mining_jobs (galaxy_id, station_id);

grant select on planetary_mining_jobs to public;

update version_number set version_number=272, min_version_number=272;
