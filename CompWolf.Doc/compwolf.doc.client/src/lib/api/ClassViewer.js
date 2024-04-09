import { Fragment } from "react";
import { Declaration, FormattedText, Reference } from "./CodeComponents";
import BaseEntityViewer from "./BaseEntityViewer";

export default function ClassViewer({ data }) {
	const is_empty = (x) => { return x == undefined || x.length == 0 }

	function MemberReference({ name }) {
		return (
			<Reference path={`${data.project}/${data.header}/${data.name}/${name}`}>
				{name}
			</Reference>
		)
	}

	return (
		<BaseEntityViewer data={data} top={
			<Fragment>
				<big id="Declaration">
					<Declaration>{data.declaration}</Declaration>
				</big>
				<p id="Description">
					<FormattedText>{data.detailedDescription}</FormattedText>
				</p>
			</Fragment>
		}>
			<section hidden={is_empty(data.dataMembers)} id="DataMembers">
				<h2>Data Members</h2>
				<table className="memberTable">
					<thead>
						<tr>
							<th>Name</th>
							<th>Description</th>
						</tr>
					</thead>
					<tbody>
						{data.dataMembers?.map((x, i) => {
							return (
								<tr key={i}>
									<td>
										<MemberReference name={x.name} />
									</td>
									<td>
										<FormattedText>{x.briefDescription}</FormattedText>
									</td>
								</tr>
							)
						})}
					</tbody>
				</table>
			</section>

			<section hidden={is_empty(data.methodGroups) && !data.constructor} id="MemberFunctions">
				<h2>Member Functions</h2>
				<table className="memberTable">
					<thead>
						<tr>
							<th>Name</th>
							<th>Description</th>
						</tr>
					</thead>
					<tbody>
						<tr hidden={!data.constructor}>
							<td>
								<Reference path={`${data.project}/${data.header}/${data.name}/${data.name}`}>
									(constructors)
								</Reference>
							</td>
							<td>
								{data.constructor?.briefDescription}
							</td>
						</tr>
						{data.methodGroups?.map((x, i) => {
							return (
								<Fragment key={i}>
									<tr>
										<td colSpan="2">
											<h3>{x.name}</h3>
											<p>
												<FormattedText>{x.description}</FormattedText>
											</p>
										</td>
									</tr>
									{x.items.map((x, i) => {
										return (
											<tr key={i}>
												<td>
													<MemberReference name={x.name} />
												</td>
												<td>
													<FormattedText>{x.briefDescription}</FormattedText>
												</td>
											</tr>
										)
									})}
								</Fragment>
							)
						})}
					</tbody>
				</table>
			</section>
		</BaseEntityViewer>
	)
}